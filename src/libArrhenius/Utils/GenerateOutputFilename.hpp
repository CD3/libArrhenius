
#ifndef Utils_GenerateOutputFilename_hpp
#define Utils_GenerateOutputFilename_hpp

#include <regex>
#include <string>

#include "./Tokenizer.hpp"

/** @file GenerateOutputFilename.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 07/10/17
  */

namespace RUC {
  /** @brief Generate an output filename from an input filename and a transform string.
   *
   * This function is useful for generate filenames to write output data to from an input filename.
   * Several methods for generate the output filename are supported including a regex replace based
   * on the input filename, a format string similar to (but much less powerful than)
   * python's string.format, and a simple name.
   *
   * Dependencies: Requires Tokenize.hpp
   *
   * @param input_filename The input filename that corresponds to the output file.
   * @param transform_string A string specifying how the output filename should be generated.
   * @param context An (optional) map containing the context to use for formatting the output filename.
   * @return the generated output filename.
   *
   * transform strings:
   * 
   * Transform methods are specified with a code at the beginning of the transform_string. Currently
   * supported codes are:
   *
   * re: - regular expression replace transform
   * fmt: - simple {var} replacement.
   *
   *   regex transform:
   *
   *   A regular expression transform is specified with the code "re:". The search pattern
   *   and replacement are appended to the transform code, separated by a ':'.
   *   For example:
   *
   *   "re:pattern:replace"
   *
   *   Will replace the first occurrence of "pattern" with "replace". The pattern is a regular
   *   expression, so sophisticated patterns can be given, including back references.
   *
   *   format transform:
   *
   *   A simple format transform is specified with the code "fmt:". The format
   *   string will be "rendered" to generate the output filename. Any occurance
   *   of the string "{ifn}" in the format string will be replaced with the input filename.
   *   So, for example:
   *
   *   "fmt:{ifn}.new_ext"
   *
   *   Would simply tack on ".new_ext" to the input filename.
   *
   * If no format code is specified, the transform string is returned as the output filename.
   *
   * An exception will be thrown if the output filename is identical to the input filename, or
   * if the generated output filename is empty.
   *
   */

std::string GenerateOutputFilename( std::string input_filename, std::string transform_string )
{
  std::string ofn;

  if( transform_string.find("re:") == 0 )
  {
    auto tokens = RUC::Tokenize( transform_string, ":" );
    std::regex re(tokens[1]);
    ofn = std::regex_replace(input_filename,re,tokens[2]); 
  }
  else if( transform_string.find("fmt:") == 0 )
  {
    auto tokens = RUC::Tokenize( transform_string, ":" );
    std::regex re("\\{ifn\\}");
    ofn = std::regex_replace( tokens[1], re, input_filename );
  }
  else
  {
    ofn = transform_string;
  }


  if( ofn == input_filename )
    throw std::runtime_error("ERROR: output filename "+ofn+" is same as input filename.");

  if( ofn == "" )
    throw std::runtime_error("ERROR: output filename is empty.");


  return ofn;
}
}


#endif // include protector
