#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

#include "Arrhenius.hpp"

namespace po = boost::program_options;
using namespace std;

typedef double DataType;

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

int main(int argc, const char** argv)
{
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
      ("help,h",    "print help message.")
      ("version", "print library version.")
      ("manual",    "print manual.")
      ("verbose,v", po::value<int>()->implicit_value(0), "verbose level.") // an option that takes an argument, but has a default value.
      ("Ea", po::value<DataType>()->default_value(6.28e5), "Activation energy.")
      ("A", po::value<DataType>()->default_value(3.1e99), "Frequency factor.")
      ("T0", po::value<DataType>()->default_value(0), "Offset temperature that will be added to all thermal profiles.")

      ;

    // now define our arguments.
    po::options_description arg_options("Arguments");
    arg_options.add_options()
      ("files"  , po::value<std::vector<std::string>>()->composing(),   "Thermal profile files to analyze."); // an option that can be given multiple times with each argument getting stored in a vector.

    // combine the options and arguments into one option list.
    // this is what we will use to parse the command line, but
    // when we output a description of the options, we will just use
    // opt_options
    po::options_description all_options("Options");
    all_options.add(opt_options).add(arg_options);

    // tell boost how to translate positional options to named options
    po::positional_options_description args;
    args.add("files", -1);  // remaining arguments will be mapped to argument-remaining

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).  options(all_options).positional(args).run(), vm);
    po::notify(vm);


    // boiler plate stuff

    if( vm.count("help") )
    {
      // print out a usage statement and summary of command line options
      std::cout << "Example-cli [options] <file>" << "\n\n";
      std::cout << opt_options << "\n";
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


    // start application
    //
    //
    // below is an example of how to use the command line parser to access the options and arguments that
    // where passed.
    
    libArrhenius::ThresholdCalculator< libArrhenius::ArrheniusIntegral<DataType> > calc;
    calc.setA( vm["A"].as<DataType>() );
    calc.setEa( vm["Ea"].as<DataType>() );
    
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

      delete[] t;
      delete[] T;
    }
    





    return 0;
}
