#include "utils.hpp"

using str_arg = const std::string&;

std::string format_str(const std::string text, ...)
{
    va_list args;
    va_start(args, text);
    int size = vsnprintf(nullptr, 0, text.c_str(), args); // Get required size
    va_end(args); // `args` is consumed here
    if (size < 0) return "";
    std::string buffer(size, '\0'); // Allocate std::string with correct size
    va_list args2;
    va_start(args2, text);
    vsnprintf(&buffer[0], size + 1, text.c_str(), args2); // Fill buffer
    va_end(args2);
    return buffer;
}

std::string get_output_name(str_arg input_file)
{
    size_t pos = input_file.find(".");
    if (pos == -1) return input_file;
    return input_file.substr(0, pos);
}

int transcribe_audio(str_arg audioFile, str_arg transcriptFile)
{
    std::string command = "./whisper -m models/medium.en.bin -f " + audioFile + " -otxt";
    return system(command.c_str());
}

std::string read_file(str_arg filename)
{
    std::ifstream file(filename); // Open in read mode
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void write_file(str_arg filename, str_arg text)
{
    std::ofstream file(filename.c_str());
    if (file.is_open()) {
        file << text;
        return;
    }
    std::cerr << "Error opening file" << std::endl;
    exit(1);
}

std::string get_transcription(str_arg output)
{
    my::string temp = output;
    my::vector<my::string> lines = temp.split("\n");
    my::vector<my::string> temp_vec;
    for (const my::string& line : lines) {
        if (line.str().starts_with("[")) temp_vec.emplace(line);
    }
    return temp_vec.join("\n");
}

my::string convert_line(const my::string& line, size_t current)
{
    size_t end_bracket = line.str().find("]") + 1;
    my::string timestamp = line.str().substr(0, end_bracket);
    while (timestamp.contains(".")) {
        size_t pos = timestamp.str().find(".");
        timestamp[pos] = ',';
    }
    timestamp = timestamp.str().substr(1, timestamp.size()-2);
    my::string text = line.str().substr(end_bracket+1);
    text = text.trim();
    my::string result = format_str("%d\n%s\n%s", current, timestamp.str().c_str(), text.str().c_str());
    return result;
}

std::string transcription_to_srt(str_arg transcript)
{
    my::string temp = transcript;
    my::vector<my::string> lines = temp.split("\n");
    size_t current = 1;
    my::vector<my::string> temp_vec;
    for (const my::string& line : lines) {
        my::string new_line = convert_line(line, current++);
        temp_vec.emplace(new_line);
    }
    return temp_vec.join("\n\n").trim();
}

std::string get_caption_cmd(int argc, char** argv, str_arg hw_encoder, str_arg filename, str_arg encoding_flag, str_arg transcript_path)
{
    std::string result;
    if (argc > 2) {
        std::set<Flag> flags = parse_flags(argc, argv);
        if (flags.count(Flag::COMBINE)) {
            if (flags.count(Flag::BURN)) {
                result = construct_burn_cmd(hw_encoder, filename, encoding_flag, transcript_path);
            } 
            else {
                result = format_str(
                    "ffmpeg -y -i %s -i %s -c:v copy -c:a copy -c:s mov_text %s_w_captions.mp4",
                    filename.c_str(),
                    transcript_path.c_str(),
                    get_output_name(filename).c_str()
                );
            }
        }
        else if (flags.count(Flag::BURN)) result = construct_burn_cmd(hw_encoder, filename, encoding_flag, transcript_path);
        else if (flags.count(Flag::SRT_ONLY)) result = "SRT_ONLY";
        else std::cerr << "Invalid flag." << std::endl;
    }
    else {
        result = format_str(
            "ffmpeg -y -i %s -i %s -c:v copy -c:a copy -c:s mov_text %s_w_captions.mp4",
            filename.c_str(),
            transcript_path.c_str(),
            get_output_name(filename).c_str()
        );
    }
    return result;
}


// std::string get_caption_cmd(int argc, char** argv, str_arg hw_encoder, str_arg filename, str_arg encoding_flag, str_arg transcript_path)
// {
//     std::string result;
//     if (argc > 2) {
//         Flag f = parse_flags(argc, argv);
//         switch (f) {
//         case Flag::BURN:
//             result = construct_burn_cmd(hw_encoder, filename, encoding_flag, transcript_path);
//             break;
//         case Flag::SRT_ONLY:
//             result = "SRT_ONLY";
//             break;
//         case Flag::COMBINE:
            
//         default:
//             std::cout << "Invalid flag." << std::endl;
//             break;
//         }
//     }
//     else {
//         result = format_str(
//             "ffmpeg -y -i %s -i %s -c:v copy -c:a copy -c:s mov_text %s_w_captions.mp4",
//             filename.c_str(),
//             transcript_path.c_str(),
//             get_output_name(filename).c_str()
//         );
//     }
//     return result;
// }

std::string construct_burn_cmd(str_arg hw_encoder, str_arg filename, str_arg encoding_flag, str_arg transcript_path)
{
    std::string result;
    result = format_str(
        "ffmpeg -y -hwaccel %s -i %s%s-vf subtitles=%s -c:a copy %s_w_captions.mp4",
        hw_encoder.c_str(),
        filename.c_str(),
        encoding_flag.c_str(),
        transcript_path.c_str(),
        get_output_name(filename).c_str()
    );
    return result;
}

std::string get_flag(const char* arg)
{
    std::string_view flag(arg);
    return flag.starts_with('-') ? 
        std::string(flag.substr(flag.find_first_not_of("-"))) : 
        std::string(flag);
}

std::string get_extraction_cmd(str_arg hw_encoder, str_arg filename, str_arg encoding_flag, str_arg output)
{
    std::string result = format_str(
        "ffmpeg -y -hwaccel %s -i %s%s-vn -acodec pcm_s16le -ar 16000 -ac 1 %s",
        hw_encoder.c_str(),
        filename.c_str(),
        encoding_flag.c_str(),
        output.c_str()
    );
    return result;
}

void log_err_and_exit(str_arg text, str_arg err_message)
{
    std::cerr << text << std::endl;
    std::cerr << err_message << std::endl;
    exit(1);
}

std::set<Flag> parse_flags(int argc, char** argv) 
{
    std::set<Flag> result;
    for (int i = 1; i < argc; i++) {
        std::string flag = get_flag(argv[i]);
        if (flag == "burn") result.insert(Flag::BURN);
        if (flag == "srt-only") result.insert(Flag::SRT_ONLY);
        if (flag == "combine") result.insert(Flag::COMBINE);
    }
    return result;
}

std::pair<int, std::string> remove_temp_file(str_arg filepath) 
{
#if defined(OS_WINDOWS)
    std::string cmd = "del /F /Q " + filepath;
#else
    std::string cmd = "rm -f " + filepath;
#endif
    std::pair<int, std::string> res = OS::run_command(cmd);
    return res;
}
    
std::string get_transcript_path(int argc, char** argv)
{
    for (int i = 0; i < argc; i++) {
        my::string temp = argv[i];
        if (temp.contains(".srt")) return temp;
    }
    std::cerr << "Unable to locate .srt file." << std::endl;
    exit(1);
}