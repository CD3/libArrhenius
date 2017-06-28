#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/program_options.hpp>

#include "Arrhenius.hpp"

namespace po = boost::program_options;
using namespace std;

typedef double DataType;

std::vector<std::pair<std::string,std::string>> cmds = { {"help","Print usage and exit."}
                                                       , {"calc","Perform various calculations on thermal profile data"}
                                                       , {"fit","Fit Arrhenius coefficients to a set of thermal profile data."}
                                                       };

void print_usage(std::string prog, po::options_description& opts)
{
  std::cout << "Usage: " << prog << " [global options] cmd [cmd options]\n" << std::endl;
  std::cout << opts << std::endl;
  std::cout << "Commands:" << std::endl;
  for( auto c : cmds )
  {
    std::cout << "  " << c.first << "\t\t- " << c.second << std::endl;
  }
  std::cout << std::endl;
}

void print_manual()
{
  // we are using a raw string literal here (the R"()" syntax, which is a C++11 feature.
  // it allows for a multi-line string to be written directly, instead of needing to quote
  // each line or insert \n characters.
  std::cout << 
  R"(
  Under construction.
    )"
  << std::endl;
}




void calc_help(std::string prog, std::string cmd, po::options_description& opts)
{
  std::cout << "Usage: " << prog << " [global options] "<< cmd << " ["<< cmd <<" options]\n" << std::endl;
  std::cout << opts << std::endl;
  std::cout << std::endl;
}
int calc_cmd( std::string prog, std::string cmd, std::vector<std::string> &cmd_args )
{
    po::options_description opt_options("Options");
    opt_options.add_options()
      // these are simple flag options, they do not have an argument.
      ("help,h",  "print help message.")
      ("Ea", po::value<DataType>()->default_value(6.28e5), "Activation energy.")
      ("A",  po::value<DataType>()->default_value(3.1e99), "Frequency factor.")
      ("T0", po::value<DataType>()->default_value(0), "Offset temperature that will be added to all thermal profiles.")
      ("Omega", po::value<DataType>()->default_value(1), "Compute threshold corresponding to the value of Omega.")
      ("write-threshold-profiles", "Write the threshold thermal profile to disk.")
      ;
    po::options_description arg_options("Arguments");
    arg_options.add_options()
      ("files"  , po::value<std::vector<std::string>>()->composing(), "Thermal profile files to analyze.") // an option that can be given multiple times with each argument getting stored in a vector.
      ;

    po::options_description all_options("Options");
    all_options.add(opt_options).add(arg_options);

    // tell boost how to translate positional options to named options
    po::positional_options_description args;
    args.add("files"  , -1);
    
    // now actually oparse them
    po::variables_map vm;
    po::store(po::command_line_parser(cmd_args).  options(all_options).positional(args).run(), vm);
    po::notify(vm);

    if( vm.count("help") || vm.count("files") == 0 )
    {
      calc_help(prog,cmd,opt_options);
      return 0;
    }


    libArrhenius::ThresholdCalculator< libArrhenius::ArrheniusIntegral<DataType> > calc;
    calc.setA( vm["A"].as<DataType>() );
    calc.setEa( vm["Ea"].as<DataType>() );
    calc.setThresholdOmega( vm["Omega"].as<DataType>() );
    
    std::cout<< "filename | Omega | threshold" << std::endl;
    for( auto file : vm["files"].as<std::vector<std::string>>() )
    {
      int n;
      DataType *t, *T;

      std::ifstream in(file.c_str());
      RUC::ReadFunction(in, t, T, n);
      in.close();
      // add offset temp
      std::transform( T, T+n, T, std::bind2nd(std::plus<DataType>(), vm["T0"].as<DataType>()) );

      auto Omega = calc.Omega(n,t,T);
      auto Threshold = calc(n,t,T);

      std::cout << file << " | " << Omega << " | " << Threshold << std::endl;

      if( vm.count("write-threshold-profiles") )
      {
        DataType *TT = new DataType[n];
        for(size_t i = 0; i < n; ++i)
          TT[i] = Threshold*(T[i] - T[0]) + T[0];

        std::string ofn = file+".threshold";
        std::ofstream out( ofn );
        for(size_t i = 0; i < n; i++)
          out << t[i] << " " << TT[i] << "\n";
        out.close();

        delete[] TT;

      }

      delete[] t;
      delete[] T;
    }
    
    return 0;







}

void fit_help(std::string prog, std::string cmd, po::options_description& opts)
{
  std::cout << "Usage: " << prog << " [global options] "<< cmd << " ["<< cmd <<" options]\n" << std::endl;
  std::cout << opts << std::endl;
  std::cout << std::endl;
}

int fit_cmd( std::string prog, std::string cmd, std::vector<std::string> &cmd_args )
{
    po::options_description opt_options("Options");
    opt_options.add_options()
      // these are simple flag options, they do not have an argument.
      ("help,h",  "print help message.")
      ("methods,m"  , po::value<std::vector<std::string>>()->composing(), "List of fitting methods to use. Coefficients for each method will be printed.")
      ;
    po::options_description arg_options("Arguments");
    arg_options.add_options()
      ("files"  , po::value<std::vector<std::string>>()->composing(), "Thermal profile files to fit. These are ASSUMED to be threshold profiles.")
      ;

    po::options_description all_options("Options");
    all_options.add(opt_options).add(arg_options);

    // tell boost how to translate positional options to named options
    po::positional_options_description args;
    args.add("files"  , -1);
    
    // now actually oparse them
    po::variables_map vm;
    po::store(po::command_line_parser(cmd_args).  options(all_options).positional(args).run(), vm);
    po::notify(vm);

    if( vm.count("help") || vm.count("files") == 0 )
    {
      calc_help(prog,cmd,opt_options);
      return 0;
    }

    std::vector<std::pair<std::string,std::string>> methods = {
      {"Effective Exposure", "Computes an 'effective exposure' for each thermal profile and performs the standard linear regression method on them."}
     ,{"Minimize log(A) Std Dev and Scaling Factors","Finds Ea that minimizes variance in A, then find A that minimizes required scaling factor."}
    };

    for( auto m : vm["methods"].as<std::vector<std::string>>())
    {
    }


    
    return 0;







}





int main(int argc, const char** argv)
{
    std::vector<std::string> global_args;
    std::vector<std::string> cmd_args;

    bool cmd_found = false;
    for( int i = 1; i < argc; i++ )
    {
      std::string arg( argv[i] );
      if(!cmd_found)
        global_args.push_back( arg );

      if(cmd_found)
        cmd_args.push_back( arg );

      for( auto c : cmds )
      {
        if( c.first == arg )
        {
          cmd_found = true;
          break;
        }
      }
    }

    // setup command line parser for the global arguments.
    //
    // note that boost is a little different...
    // we usually say that a command line program takes options and arguments.
    // boost calls arguments "positional options", which are just options that can
    // be given without their option name. to create the argument, we define it as
    // an option first, and then tell boost that it is "positional"
    //
    // so to define our command line interface
    // we declare options and arguments that our application will accept.

    // define our regular options. these will be given with a '--' or '-' in front. '--' is used
    // for the long name, '-' is used for the short name.
    po::options_description opt_options("Options");
    opt_options.add_options()
      // these are simple flag options, they do not have an argument.
      ("help,h",  "print help message.")
      ("version", "print library version.")
      ("manual",  "print manual.")
      ("verbose,v", po::value<int>()->implicit_value(0), "verbose level.") // an option that takes an argument, but has a default value.
      ;

    // now define our arguments.
    po::options_description arg_options("Arguments");
    arg_options.add_options()
      ("command"  , po::value<std::string>()->default_value("help"),   "Subcommand.");

    // combine the options and arguments into one option list.
    // this is what we will use to parse the command line, but
    // when we output a description of the options, we will just use
    // opt_options
    po::options_description all_options("Options");
    all_options.add(opt_options).add(arg_options);

    // tell boost how to translate positional options to named options
    po::positional_options_description args;
    args.add("command", 1);

    // if no subcommand was found, we need to print the help before the parser
    // tries to parse because it will likely just fail.
    // note that we can't do this above because we need the options descriptor
    if( !cmd_found )
    {
      // print out a usage statement and summary of command line options
      std::cout << "ERROR: No subcommand found." << std::endl;
      print_usage(argv[0],opt_options);
      return 1;
    }

    po::variables_map vm;
    po::store(po::command_line_parser(global_args).  options(all_options).positional(args).run(), vm);
    po::notify(vm);


    // boiler plate stuff

    if( vm.count("help") || vm["command"].as<std::string>() == "help" )
    {
      // print out a usage statement and summary of command line options
      print_usage(argv[0],opt_options);
      return 0;
    }

    if( vm.count("manual") )
    {
      // print the manual
      print_manual();
      return 0;
    }

    if( vm.count("version") )
    {
      // print the version number for the library
      std::cout << "libArrhenius "<<libArrhenius_VERSION_FULL << std::endl;
      return 0;
    }

    if( vm["command"].as<std::string>() == "calc" )
      return calc_cmd( argv[0], "calc", cmd_args );

    if( vm["command"].as<std::string>() == "fit" )
      return fit_cmd( argv[0], "fit", cmd_args );

    


    return 0;
}
