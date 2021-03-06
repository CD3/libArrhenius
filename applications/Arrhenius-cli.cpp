#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>

#include <boost/filesystem.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
namespace logging = boost::log;

#include <boost/program_options.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
using namespace boost::multiprecision;

#include <libArrhenius/version.h>
#include <libArrhenius/Arrhenius.hpp>
#include <libArrhenius/Utils/GenerateOutputFilename.hpp>

namespace po = boost::program_options;
using namespace std;

typedef cpp_dec_float_100 DataType;
typedef cpp_dec_float_100 HPDataType; // high precision data type

namespace std {
  /* std::string to_string(cpp_dec_float_100 val) */
  /* {return static_cast<std::string>( val ); } */

  /* std::string to_string(cpp_dec_float_50 val) */
  /* {return static_cast<std::string>( val ); } */
}

std::vector<std::pair<std::string,std::string>> cmds = { {"help","Print usage and exit."}
                                                       , {"calc-threshold","Calculate the threshold scaling factor for a thermal profile(s)."}
                                                       , {"calc-rate","Calculate the damage rate for a thermal profile(s)"}
                                                       , {"calc-damage","Calculate the damage parameter for a thermal profile(s)"}
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





void calc_threshold_help(std::string prog, std::string cmd, po::options_description& opts)
{
  std::cout << "Usage: " << prog << " [global options] "<< cmd << " ["<< cmd <<" options]\n" << std::endl;
  std::cout << opts << std::endl;
  std::cout << std::endl;
}
int calc_threshold_cmd( std::string prog, std::string cmd, std::vector<std::string> &cmd_args )
{
    po::options_description opt_options("Options");
    opt_options.add_options()
      // these are simple flag options, they do not have an argument.
      ("help,h",  "print help message.")
      ("Ea", po::value<DataType>()->default_value(6.28e5), "Activation energy.")
      ("A",  po::value<DataType>()->default_value(3.1e99), "Frequency factor.")
      ("n",  po::value<DataType>()->default_value(0), "Temperature pre-factor exponent for modified Arrhenius equation.")
      ("T0", po::value<DataType>()->default_value(0), "Offset temperature that will be added to all thermal profiles.")
      ("Omega", po::value<DataType>()->default_value(1), "Compute threshold corresponding to the value of Omega.")
      ("write-threshold-profiles,w", "Write the threshold thermal profile to disk.")
      ("output-filename,o", po::value<std::string>()->default_value("fmt:{ifn}.threshold"), "Output filename.")
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
    
    // now actually parse them
    po::variables_map vm;
    po::store(po::command_line_parser(cmd_args).  options(all_options).positional(args).run(), vm);
    po::notify(vm);

    if( vm.count("help") || vm.count("files") == 0 )
    {
      calc_threshold_help(prog,cmd,opt_options);
      return 0;
    }


    libArrhenius::ThresholdCalculator< libArrhenius::ModifiedArrheniusIntegral<DataType> > calc;
    calc.setA( vm["A"].as<DataType>() );
    calc.setEa( vm["Ea"].as<DataType>() );
    calc.setExponent( vm["n"].as<DataType>() );
    calc.setThresholdOmega( vm["Omega"].as<DataType>() );
    
    std::cout<< "filename | Omega | threshold" << std::endl;
    for( auto file : vm["files"].as<std::vector<std::string>>() )
    {
      int n;
      DataType *t, *T;

      if( !boost::filesystem::exists(file) )
        throw std::runtime_error("ERROR: '"+file+"' does not exist.");
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

        std::string ofn = RUC::GenerateOutputFilename(file,vm["output-filename"].as<std::string>());
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

void calc_rate_help(std::string prog, std::string cmd, po::options_description& opts)
{
  std::cout << "Usage: " << prog << " [global options] "<< cmd << " ["<< cmd <<" options]\n" << std::endl;
  std::cout << opts << std::endl;
  std::cout << "Use this command to calculate some statistics on the damage rate for a set of thermal profiles." << std::endl;
  std::cout << "For example, this command will print the minimum, maximum, and average damage rates." << std::endl;
  std::cout << "If the --write-rate-profiles option is given, the damage rate at each point in the thermal profile will be written to file." << std::endl;
  std::cout << std::endl;
}
int calc_rate_cmd( std::string prog, std::string cmd, std::vector<std::string> &cmd_args )
{
    po::options_description opt_options("Options");
    opt_options.add_options()
      // these are simple flag options, they do not have an argument.
      ("help,h",  "print help message.")
      ("Ea", po::value<DataType>()->default_value(6.28e5), "Activation energy.")
      ("A",  po::value<DataType>()->default_value(3.1e99), "Frequency factor.")
      ("n",  po::value<DataType>()->default_value(0), "Temperature pre-factor exponent for modified Arrhenius equation.")
      ("T0", po::value<DataType>()->default_value(0), "Offset temperature that will be added to all thermal profiles.")
      ("log", "Calculate the log of the rate instead.")
      ("output-filename,o", po::value<std::string>()->default_value("fmt:{ifn}.rate"), "Output filename.")
      ("write-rate-profiles,w", "Write the rate profile to disk.")
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
    
    // now actually parse them
    po::variables_map vm;
    po::store(po::command_line_parser(cmd_args).  options(all_options).positional(args).run(), vm);
    po::notify(vm);

    if( vm.count("help") || vm.count("files") == 0 )
    {
      calc_rate_help(prog,cmd,opt_options);
      return 0;
    }

    libArrhenius::ModifiedArrheniusIntegral<DataType> integrate;
    integrate.setA( vm["A"].as<DataType>() );
    integrate.setEa( vm["Ea"].as<DataType>() );
    integrate.setExponent( vm["n"].as<DataType>() );


    for( auto file : vm["files"].as<std::vector<std::string>>() )
    {
      int n;
      DataType *t, *T;

      if( !boost::filesystem::exists(file) )
        throw std::runtime_error("ERROR: '"+file+"' does not exist.");
      std::ifstream in(file.c_str());
      RUC::ReadFunction(in, t, T, n);
      in.close();
      // add offset temp
      std::transform( T, T+n, T, std::bind2nd(std::plus<DataType>(), vm["T0"].as<DataType>()) );

      // just reuse temperature array for rate
      for(int i = 0; i < n; i++)
      {
        if( vm.count("log") )
          T[i] = log(integrate.rate(T[i]));
        else
          T[i] = integrate.rate(T[i]);
      }

      std::cout << "file: " << file << std::endl;
      // min rate
      std::cout << "min rate: " << *std::min_element(T, T+n) << std::endl;
      // max rate
      std::cout << "max rate: " << *std::max_element(T, T+n) << std::endl;
      // avg rate
      std::cout << "avg rate: " << std::accumulate(T, T+n, DataType(0))/n << std::endl;
      // critical temperature
      DataType Tcrit = integrate.getCriticalTemperature();
      std::cout << "T crit: " <<  Tcrit << std::endl;
      // time above critical temp
      DataType taucrit = 0;
      for(int i = 1; i < n; i++)
      {
        if( (T[i] + T[i-1])/2 > 1 )
          taucrit += t[i] - t[i-1];
      }
      std::cout << "tau crit: " <<  taucrit<< std::endl;

      std::cout << std::endl;

      if( vm.count("write-rate-profiles") )
      {
        std::string ofn = RUC::GenerateOutputFilename(file,vm["output-filename"].as<std::string>());
        std::ofstream out( ofn );
        for(size_t i = 0; i < n; i++)
          out << t[i] << " " << T[i] << "\n";
        out.close();
      }


      delete[] t;
      delete[] T;
    }
    
    return 0;







}

void calc_damage_help(std::string prog, std::string cmd, po::options_description& opts)
{
  std::cout << "Usage: " << prog << " [global options] "<< cmd << " ["<< cmd <<" options]\n" << std::endl;
  std::cout << opts << std::endl;
  std::cout << "Use this command to evaluate the Arrhenius integral for on a set of thermal profiles." << std::endl;
  std::cout << "If the --write-damage-profiles option is given, the damage accumulation for each time point in the profile," << std::endl;
  std::cout << "i.e. Omega(t), will be written to a file." << std::endl;
  std::cout << std::endl;
}
int calc_damage_cmd( std::string prog, std::string cmd, std::vector<std::string> &cmd_args )
{
    po::options_description opt_options("Options");
    opt_options.add_options()
      // these are simple flag options, they do not have an argument.
      ("help,h",  "print help message.")
      ("Ea", po::value<DataType>()->default_value(6.28e5), "Activation energy.")
      ("A",  po::value<DataType>()->default_value(3.1e99), "Frequency factor.")
      ("n",  po::value<DataType>()->default_value(0), "Temperature pre-factor exponent for modified Arrhenius equation.")
      ("T0", po::value<DataType>()->default_value(0), "Offset temperature that will be added to all thermal profiles.")
      ("output-filename,o", po::value<std::string>()->default_value("fmt:{ifn}.damage"), "Output filename.")
      ("write-damage-profiles,w", "Write the damage profiles to disk.")
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
    
    // now actually parse them
    po::variables_map vm;
    po::store(po::command_line_parser(cmd_args).  options(all_options).positional(args).run(), vm);
    po::notify(vm);

    if( vm.count("help") || vm.count("files") == 0 )
    {
      calc_damage_help(prog,cmd,opt_options);
      return 0;
    }

    libArrhenius::ModifiedArrheniusIntegral<DataType> integrate;
    integrate.setA( vm["A"].as<DataType>() );
    integrate.setEa( vm["Ea"].as<DataType>() );
    integrate.setExponent( vm["n"].as<DataType>() );


    std::cout<< "filename | Omega" << std::endl;
    for( auto file : vm["files"].as<std::vector<std::string>>() )
    {
      int n;
      DataType *t, *T;

      if( !boost::filesystem::exists(file) )
        throw std::runtime_error("ERROR: '"+file+"' does not exist.");
      std::ifstream in(file.c_str());
      RUC::ReadFunction(in, t, T, n);
      in.close();
      // add offset temp
      std::transform( T, T+n, T, std::bind2nd(std::plus<DataType>(), vm["T0"].as<DataType>()) );

      auto Omega = integrate(n,t,T);
      std::cout << file << " | " << Omega << std::endl;

      if( vm.count("write-damage-profiles") )
      {
        std::string ofn = RUC::GenerateOutputFilename(file,vm["output-filename"].as<std::string>());
        std::ofstream out( ofn );
        // this is NOT efficient
        for(size_t i = 0; i < n; i++)
          out << t[i] << " " << integrate(i,t,T) << "\n";
        out.close();
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
      ("list-methods,l",  "print list of available fit methods.")
      ("T0", po::value<HPDataType>()->default_value(0), "Offset temperature (in K) that will be added to all thermal profiles.")
      ("T0-uncertainty", po::value<HPDataType>(), "Uncertainty in baseline temperature (in K).")
      ("dT-uncertainty", po::value<HPDataType>(), "Uncertainty in temperature rise (in K).")
      ("Ea-min", po::value<HPDataType>(), "Minimum bound on Ea for methods that perform a search.")
      ("Ea-max", po::value<HPDataType>(), "Maximum bound on Ea for methods that perform a search.")
      ;
    po::options_description arg_options("Arguments");
    arg_options.add_options()
      ("files"  , po::value<std::vector<std::string>>()->composing(), "Thermal profile (in K vs. s) files to fit. These are ASSUMED to be threshold profiles.")
      ;

    po::options_description all_options("Options");
    all_options.add(opt_options).add(arg_options);

    // tell boost how to translate positional options to named options
    po::positional_options_description args;
    args.add("files"  , -1);
    
    // now actually parse them
    po::variables_map vm;
    po::store(po::command_line_parser(cmd_args).  options(all_options).positional(args).run(), vm);
    po::notify(vm);

    std::vector<std::pair<std::string,std::string>> supported_methods = {
     {"Minimize log(A) Variance and Scaling Factors"," - Finds Ea that minimizes variance in log(A), then finds A that minimizes required scaling factor errors."}
    ,{"Effective Exposures","                          - Computes an 'effective exposure' for each thermal profile and performs the standard linear regression method on them."}
    };

    if( vm.count("list-methods") )
    {
      std::cout << "\n\tMethods:\n" << std::endl;
      for( auto m : supported_methods )
      {
        std::cout << "\t\t" << m.first << m.second << std::endl;
      }
      return 0;
    }


    if( vm.count("help") || vm.count("files") == 0 )
    {
      calc_threshold_help(prog,cmd,opt_options);
      return 0;
    }


    // read in thermal profiles
    std::cout << "Loading thermal profiles." << std::endl;
    std::vector<std::shared_ptr<HPDataType>> ts,Ts;
    std::vector<size_t> Ns;
    for( auto file : vm["files"].as<std::vector<std::string>>() )
    {
      int n;
      HPDataType *t, *T;

      if( !boost::filesystem::exists(file) )
        throw std::runtime_error("ERROR: '"+file+"' does not exist.");
      std::ifstream in(file.c_str());
      RUC::ReadFunction(in, t, T, n);
      in.close();
      // add offset temp
      std::transform( T, T+n, T, std::bind2nd(std::plus<HPDataType>(), vm["T0"].as<HPDataType>()) );

      // we need to wrap these in shared_ptr's so we don't leak memory
      Ns.push_back(n);
      ts.push_back( std::shared_ptr<HPDataType>( t ) );
      Ts.push_back( std::shared_ptr<HPDataType>( T ) );

    }







    std::vector<std::string> methods;
    if( vm.count("methods") )
    {
      for( auto m : vm["methods"].as<std::vector<std::string>>() )
        methods.push_back(m);
    }
    else
    {
      for( auto m : supported_methods )
        methods.push_back(m.first);
    }


    // loop through various fit methods
    for( auto m : methods )
    {
      std::transform( m.begin(), m.end(), m.begin(), ::tolower);

      std::cout << "Running " << m << " method." << std::endl;

      typedef libArrhenius::ArrheniusFitInterface<HPDataType>::Return Coeffs;
      Coeffs coefficients;
      Coeffs coefficients_err;

      std::shared_ptr< libArrhenius::ArrheniusFitInterface< HPDataType> > fit;

      // support aliases
      if(m == "minimize log(a) variance and scaling factors")
        m = "clark";

      if(m == "effective exposures")
        m = "denton";

      if(m == "effective exposures linear regression")
        m = "denton";


      // get the correct fitter
      if(m == "clark")
        fit.reset( new libArrhenius::ArrheniusFit< HPDataType, libArrhenius::MinimizeLogAVarianceAndScalingFactors >());

      if(m == "denton")
        fit.reset( new libArrhenius::ArrheniusFit< HPDataType, libArrhenius::EffectiveExposuresLinearRegression>());

      // now fit the profiles
      for( int i = 0; i < Ns.size(); ++i )
        fit->addProfile( Ns[i], ts[i].get(), Ts[i].get() );

      if(vm.count("Ea-min"))
        fit->setMinEa( vm["Ea-min"].as<HPDataType>() );
      if(vm.count("Ea-max"))
        fit->setMaxEa( vm["Ea-max"].as<HPDataType>() );

      coefficients = fit->exec();

      std::vector< Coeffs > errors;

      // propagate error for uncertainty in baseline temperature if given
      if( vm.count("T0-uncertainty") )
      {
        fit->clear();
        // add uncertainty to the thermal profiles and add them to the fitter
        for( int i = 0; i < Ns.size(); ++i )
        {
          std::transform( Ts[i].get(), Ts[i].get()+Ns[i], Ts[i].get(), std::bind2nd(std::plus<HPDataType>(), vm["T0-uncertainty"].as<HPDataType>()) );
          fit->addProfile( Ns[i], ts[i].get(), Ts[i].get() );
        }

        errors.push_back(fit->exec());

        // subtract the uncertainty back off the thermal profiles
        for( int i = 0; i < Ns.size(); ++i )
          std::transform( Ts[i].get(), Ts[i].get()+Ns[i], Ts[i].get(), std::bind2nd(std::minus<HPDataType>(), vm["T0-uncertainty"].as<HPDataType>()) );
      }

      // propagate error for peak temperature rise if given
      if( vm.count("dT-uncertainty") )
      {
        fit->clear();
        // add uncertainty to the thermal profiles and add them to the fitter
        for( int i = 0; i < Ns.size(); ++i )
        {
          HPDataType Tmax = *std::max_element( Ts[i].get(), Ts[i].get() + Ns[i] );
          HPDataType scale = 1 + vm["dT-uncertainty"].as<HPDataType>()/(Tmax - Ts[i].get()[0]);
          for( int j = 0; j < Ns[i]; j++)
            Ts[i].get()[j] = Ts[i].get()[0] + scale*(Ts[i].get()[j] - Ts[i].get()[0]);

          fit->addProfile( Ns[i], ts[i].get(), Ts[i].get() );
        }

        errors.push_back(fit->exec());

        // subtract the uncertainty back off the thermal profiles
        for( int i = 0; i < Ns.size(); ++i )
        {
          HPDataType Tmax = *std::max_element( Ts[i].get(), Ts[i].get() + Ns[i] );
          HPDataType scale = 1 - vm["dT-uncertainty"].as<HPDataType>()/(Tmax - Ts[i].get()[0]);
          for( int j = 0; j < Ns[i]; j++)
            Ts[i].get()[j] = Ts[i].get()[0] + scale*(Ts[i].get()[j] - Ts[i].get()[0]);
        }
      }


      // compute total error
      coefficients_err.A = 0;
      coefficients_err.Ea = 0;
      for( auto e : errors )
      {
        auto dA  = e.A.get() - coefficients.A.get();
        auto dEa = e.Ea.get() - coefficients.Ea.get();
        coefficients_err.A  = coefficients_err.A.get()  + dA*dA;
        coefficients_err.Ea = coefficients_err.Ea.get() + dEa*dEa;
      }
      coefficients_err.A  = sqrt(coefficients_err.A.get());
      coefficients_err.Ea = sqrt(coefficients_err.Ea.get());


      std::cout << "A: " << coefficients.A.get()   << " +/- " << coefficients_err.A.get() << std::endl;
      std::cout << "Ea: " << coefficients.Ea.get() << " +/- " << coefficients_err.Ea.get() << std::endl;

      std::cout<< "filename | Omega | threshold" << std::endl;

      // todo: should we add support for modified arrhenius?
      libArrhenius::ThresholdCalculator< libArrhenius::ArrheniusIntegral<DataType> > calc;
      calc.setA( coefficients.A.get() );
      calc.setEa( coefficients.Ea.get() );
      //calc.setExponent( vm["n"].as<DataType>() );
      // what about support for omega != 1?
      //calc.setThresholdOmega( vm["Omega"].as<DataType>() );
      std::vector<HPDataType> Thresholds(Ns.size());
      for( int i = 0; i < vm["files"].as<std::vector<std::string>>().size(); i++ )
      {
        auto file = vm["files"].as<std::vector<std::string>>()[i];
        auto Omega = calc.Omega(Ns[i],ts[i].get(),Ts[i].get());
        auto Threshold = calc(Ns[i],ts[i].get(),Ts[i].get());
        std::cout << file << " | " << Omega << " | " << Threshold << std::endl;
        Thresholds[i] = Threshold;
      }
      // calc sum of residuals
      std::cout << "R^2: " << std::accumulate(Thresholds.begin(), Thresholds.end(), HPDataType(0), []( HPDataType a, HPDataType b ){ return HPDataType(a + (b-1)*(b-1)); } ) << std::endl;

      std::cout << std::endl;

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
      ("verbose,v", po::value<int>()->default_value(0), "verbose level.") // an option that takes an argument, but has a default value.
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

    logging::core::get()->set_filter( logging::trivial::severity >= logging::trivial::error );
    if( vm["verbose"].as<int>() > 0 )
      logging::core::get()->set_filter( logging::trivial::severity >= logging::trivial::warning );
    if( vm["verbose"].as<int>() > 1 )
      logging::core::get()->set_filter( logging::trivial::severity >= logging::trivial::info );
    if( vm["verbose"].as<int>() > 2 )
      logging::core::get()->set_filter( logging::trivial::severity >= logging::trivial::debug );
    if( vm["verbose"].as<int>() > 3 )
      logging::core::get()->set_filter( logging::trivial::severity >= logging::trivial::trace );




    if( vm["command"].as<std::string>() == "calc-threshold" )
      return calc_threshold_cmd( argv[0], "calc-threshold", cmd_args );

    if( vm["command"].as<std::string>() == "calc-rate" )
      return calc_rate_cmd( argv[0], "calc-rate", cmd_args );

    if( vm["command"].as<std::string>() == "calc-damage" )
      return calc_damage_cmd( argv[0], "calc-damage", cmd_args );

    if( vm["command"].as<std::string>() == "fit" )
      return fit_cmd( argv[0], "fit", cmd_args );

    


    return 0;
}
