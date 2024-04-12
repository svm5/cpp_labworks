#include "parse_arguments.cpp"
#include "sandpile.cpp"

int main(int argc, char** argv) {
   ArgParseResult parsing_result;
   ArgParseResultType type = parsing_result.ParseArguments(argc, argv);
   if (type != ArgParseResultType::Ok) {
     EndProgramWithError(parsing_result.type);
   }

   Sandpile s;
   s.CreateSandpile(parsing_result.options.input_filename);
   s.Spilling(parsing_result.options.output_directory, 
              parsing_result.options.max_iterations, 
              parsing_result.options.save_frequency);
   if (parsing_result.options.save_frequency == 0) {
      char* path = CreateFilename(parsing_result.options.output_directory, kWithoutIndex);
      s.Draw(path);
      delete[] path;
   }
   return EXIT_SUCCESS;
}
