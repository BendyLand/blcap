#pragma once

#include <cstdarg>
#include <fstream>
#include "mystl.hpp"
#include "os.hpp"

using str_arg = const std::string&;

enum class Flag 
{
    BURN,
    SRT_ONLY,
    SOFT,
    INVALID,
};

std::string format_str(const std::string text, ...);
std::string get_output_name(str_arg input_file);
int transcribe_audio(str_arg audioFile, str_arg transcriptFile);
std::string read_file(str_arg filename);
void write_file(str_arg filename, str_arg text);
std::string get_transcription(str_arg output);
my::string convert_line(const my::string& line, size_t current);
std::string transcription_to_srt(str_arg transcript);
std::string get_caption_cmd(int argc, char** argv, str_arg hw_encoder, str_arg filename, str_arg encoding_flag, str_arg transcript_path);
std::string construct_burn_cmd(str_arg hw_encoder, str_arg filename, str_arg encoding_flag, str_arg transcript_path);
std::string get_flag(const char* arg);
std::string get_extraction_cmd(str_arg hw_encoder, str_arg filename, str_arg encoding_flag, str_arg output);
void log_err_and_exit(str_arg text, str_arg err_message);
std::pair<int, std::string> remove_temp_file(str_arg filepath);
Flag parse_flags(int argc, char** argv);
