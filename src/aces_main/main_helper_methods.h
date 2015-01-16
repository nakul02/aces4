/*
 * main_helper_methods.h
 *
 *  Created on: Oct 18, 2014
 *      Author: njindal
 */

#ifndef MAIN_HELPER_METHODS_H_
#define MAIN_HELPER_METHODS_H_

#include "config.h"
#include "siox_reader.h"
#include "io_utils.h"
#include "setup_reader.h"
#include "assert.h"
#include "sialx_timer.h"
#include "sip_tables.h"
#include "interpreter.h"
#include "sialx_interpreter.h"
#include "setup_interface.h"
#include "sip_interface.h"
#include "data_manager.h"
#include "worker_persistent_array_manager.h"
#include "block.h"
#include "global_state.h"
#include "sip_mpi_attr.h"
#include "rank_distribution.h"

#include <vector>
#include <sstream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fenv.h>
#include <ctime>

#include <execinfo.h>
#include <signal.h>

#ifdef HAVE_MPI
#include "mpi.h"
#include "sip_server.h"
#include "sip_mpi_utils.h"
#include "server_persistent_array_manager.h"
#endif

#ifdef HAVE_TAU
#include <TAU.h>
#endif

/**
 * http://stackoverflow.com/questions/3151779/how-its-better-to-invoke-gdb-from-program-to-print-its-stacktrace/4611112#4611112
 */
void bt_sighandler(int signum) {
	std::cerr << "Interrupt signal (" << signum << ") received." << std::endl;
    sip_abort();
}

/**
 * Prints usage of the aces4 program(s)
 * @param program_name
 */
static void print_usage(const std::string& program_name) {
	std::cerr << "Usage : " << program_name << " -d <init_data_file> -j <init_json_file> -s <sialx_files_directory> -m <max_memory_in_gigabytes>" << std::endl;
	std::cerr << "\t -d : binary initialization data file " << std::endl;
	std::cerr << "\t -j : json initialization file, use EITHER -d or -j " << std::endl;
	std::cerr << "\t -s : directory of compiled sialx files  " << std::endl;
	std::cerr << "\t -m : memory to be used by workers (and servers) specified in GB " << std::endl;
	std::cerr << "\t -w : number of workers  " << std::endl;
	std::cerr << "\t -r : number of servers  " << std::endl;
	std::cerr << "\tDefaults: data file - \"data.dat\", sialx directory - \".\", Memory : 2GB" << std::endl;
	std::cerr << "\tm is the approximate memory to use. Actual usage will be more." << std::endl;
	std::cerr << "\tworkers & servers are distributed in a 2:1 ratio for an MPI build" << std::endl;
	std::cerr << "\t-? or -h to display this usage dialogue" << std::endl;
}

/**
 * Enables floating point exceptions & registers
 * signal handlers so as to print a stack trace
 * when the program exits due to failure of some kind
 */
static void setup_signal_and_exception_handlers() {
#ifdef __GLIBC__
#ifdef __GNU_LIBRARY__
#ifdef _GNU_SOURCE
    feenableexcept(FE_DIVBYZERO);
    feenableexcept(FE_OVERFLOW);
    feenableexcept(FE_INVALID);
#endif // _GNU_SOURCE
#endif // __GNU_LIBRARY__
#endif // __GLIBC__
	signal(SIGSEGV, bt_sighandler);
	signal(SIGFPE, bt_sighandler);
	signal(SIGTERM, bt_sighandler);
	signal(SIGINT, bt_sighandler);
	signal(SIGABRT, bt_sighandler);
}


/**
 * IFDEF wrapped initialization of MPI.
 * The mpi error handler is also set
 * to return the error code instead of having
 * it fail implicitly
 * @param argc
 * @param argv
 */
static void mpi_init(int *argc, char ***argv){
#ifdef HAVE_MPI
	/* MPI Initialization */
	MPI_Init(argc, argv);
#endif //HAVE_MPI
}

/**
 * IFDEF wrapped finalization of MPI
 */
static void mpi_finalize(){
#ifdef HAVE_MPI
	MPI_Finalize();
#endif
}

/**
 * IFDEF wrapped mpi barrier
 */
static void barrier(){
#ifdef HAVE_MPI
      sip::SIPMPIUtils::check_err(MPI_Barrier(MPI_COMM_WORLD));
#endif
}

/**
 * Check sizes of data types.
 * In the MPI version, the TAG is used to communicate information
 * The various bits needed to send information to other nodes
 * sums up to 32.
 */
void check_expected_datasizes() {

	sip::check(sizeof(int) >= 4, "Size of integer should be 4 bytes or more");
	sip::check(sizeof(double) >= 8, "Size of double should be 8 bytes or more");
	sip::check(sizeof(long long) >= 8,
			"Size of long long should be 8 bytes or more");
}

/**
 * Encapsulates all the parameters the user passes into the
 * aces4 executable(s)
 */
struct Aces4Parameters{
	char * init_binary_file;
	char * init_json_file;
	char * sialx_file_dir;
	bool init_json_specified ;
	bool init_binary_specified;
	std::size_t memory;
	std::string job;
	int num_workers;
	int num_servers;
	Aces4Parameters(){
		init_binary_file = "data.dat";	// Default initialization file is data.dat
		sialx_file_dir = ".";			// Default directory for compiled sialx files is "."
		init_json_file = NULL;			// Json file, no default file
		memory = 2147483648;			// Default memory usage : 2 GB
		init_json_specified = false;
		init_binary_specified = false;
		job = "";
		num_workers = -1;
		num_servers = -1;
	}
};

template<typename T>
static T read_from_optarg() {
	std::string str(optarg);
	std::stringstream ss(str);
	T d;
	ss >> d;
	return d;
}

/**
 * Parses the command line arguments
 * and returns all the parameters in
 * an instance of Aces4Parameters.
 * @param argc
 * @param argv
 * @return
 */
Aces4Parameters parse_command_line_parameters(int argc, char* argv[]) {
	Aces4Parameters parameters;
	// Read about getopt here : http://www.gnu.org/software/libc/manual/html_node/Getopt.html
	// d: name of .dat file.
	// j: name of json file (must specify either of d or j, not both.
	// s: directory to look for siox files
	// m: approximate memory to be used. Actual usage will be more than this.
	// w : number of workers
	// r : number of servers
	// h & ? are for help. They require no arguments
	const char* optString = "d:j:s:m:w:r:h?";
	int c;
	while ((c = getopt(argc, argv, optString)) != -1) {
		switch (c) {
		case 'd': {
			parameters.init_binary_file = optarg;
			parameters.init_binary_specified = true;
		}
			break;
		case 'j': {
			parameters.init_json_file = optarg;
			parameters.init_json_specified = true;
		}
			break;
		case 's': {
			parameters.sialx_file_dir = optarg;
		}
			break;
		case 'm': {
			double memory_in_gb = read_from_optarg<double>();
			parameters.memory = memory_in_gb * 1024L * 1024L * 1024L;
		}
			break;
		case 'w' : {
			parameters.num_workers = read_from_optarg<int>();
		}
			break;
		case 'r' : {
			parameters.num_servers = read_from_optarg<int>();
		}
			break;
		case 'h':
		case '?':
		default:
			std::string program_name = argv[0];
			print_usage(program_name);
			exit(1);
		}
	}
	if (parameters.init_binary_specified && parameters.init_json_specified) {
		std::cerr
				<< "Cannot specify both init binary data file and json init file !"
				<< std::endl;
		std::string program_name = argv[0];
		print_usage(program_name);
		exit(1);
	}

	if (parameters.init_json_specified)
		parameters.job = parameters.init_json_file;
	else
		parameters.job = parameters.init_binary_file;

	int num_processes = sip::SIPMPIAttr::comm_world_size();
	if (parameters.num_workers == -1 || parameters.num_servers == -1){
		// Option not specified. Use default
		parameters.num_servers = num_processes / (sip::GlobalState::get_default_worker_server_ratio() + 1);
		parameters.num_workers = num_processes - parameters.num_servers;
	} else if (parameters.num_workers + parameters.num_servers != num_processes){
		std::stringstream err_ss;
		err_ss << "Number of workers (" << parameters.num_workers << ")"
				<< "and number of servers (" << parameters.num_servers << ")"
				<< "must add up to the number of MPI processes (" << num_processes << ")";
		sip::fail (err_ss.str());
	}


	return parameters;
}

/**
 * Reads the initialization file (json or binary)
 * and returns a new SetupReader instance allocated on the heap.
 * It is the callers responsibility to cleanup this instance.
 * @param parameters
 * @return
 */
setup::SetupReader* read_init_file(const Aces4Parameters& parameters) {
	setup::SetupReader *setup_reader = NULL;
	if (parameters.init_json_specified) {
		std::ifstream json_ifstream(parameters.init_json_file,
				std::ifstream::in);
		setup_reader = new setup::SetupReader(json_ifstream);
	} else {
		//create setup_file
		SIP_MASTER_LOG(
				std::cout << "Initializing data from " << parameters.job << std::endl);
		setup::BinaryInputFile setup_file(parameters.job); //initialize setup data
		setup_reader = new setup::SetupReader(setup_file);
	}
	setup_reader->aces_validate();
	return setup_reader;
}

void set_rank_distribution(const Aces4Parameters& parameters) {
#ifdef HAVE_MPI
	sip::ConfigurableRankDistribution *rank_distribution =
			new sip::ConfigurableRankDistribution(parameters.num_workers, parameters.num_servers);
	sip::SIPMPIAttr::set_rank_distribution(rank_distribution);
#endif
}


/**
 * Returns the system timestamp as a string
 * @return
 */
std::string sip_timestamp(){
	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	std::stringstream ss;
	std::string timestamp_str(asctime(timeinfo));
	return timestamp_str;
}

#endif /* MAIN_HELPER_METHODS_H_ */