#include <iostream>
#include "os.hpp"
#include "mystl.hpp"
#include "utils.hpp"

int run(int argc, char** argv);

int main(int argc, char** argv)
{
    if (argc > 1) {
        int res = run(argc, argv);
        return res;
    }
    else {
        std::cout << "Usage: blcap <video_file>" << std::endl;
        return 1;
    }
}

int run(int argc, char** argv)
{
    std::string filename = argv[1];
    std::string output = get_output_name(filename) + ".wav";
    std::string hw_encoder;
    std::string encoding_flag;
#if defined(OS_MACOS)
    hw_encoder = "auto";
    encoding_flag = " -c:v h264_videotoolbox ";
#elif defined(OS_LINUX)
    hw_encoder = "vaapi";
    encoding_flag = " -c:v h264_vaapi -vf 'format=nv12,hwupload' ";
#elif defined(OS_WINDOWS)
    hw_encoder = "qsv";
    encoding_flag = " -c:v h264_qsv ";
#else
    hw_encoder = "auto";
    encoding_flag = " ";
#endif
    std::string extraction_cmd = get_extraction_cmd(hw_encoder, filename, encoding_flag, output);
    std::pair<int, std::string> res = OS::run_command(extraction_cmd);
    if (res.first != 0) log_err_and_exit("There was a problem extracting the transcript from the video.", res.second);

    // The whisper command is: <path>/<to>/whisper-cpp/build/bin/whisper-cli -m <path>/<to>/whisper-cpp/models/ggml-base.en.bin -f <WAV_file>
    std::string whisper_cmd = read_file("whisper-cmd.txt") + output;
    res = OS::run_command(whisper_cmd);
    if (res.first != 0) log_err_and_exit("There was a problem running Whisper.", res.second);

    std::cout << "Getting transcription..." << std::endl;
    std::string transcription = get_transcription(res.second);
    std::cout << "Transcription:\n" << transcription << "\n" << std::endl;

    std::cout << "Converting to SRT format..." << std::endl;
    transcription = transcription_to_srt(transcription);
    std::cout << "Transcription converted successfully!\n" << std::endl;

    std::cout << "Writing SRT file..." << std::endl;
    write_file("transcript.srt", transcription);
    std::cout << "File written successfully!\n" << std::endl;

    // Absolute path where `transcript.srt` can be found.
    std::string transcript_path = read_file("transcript-path.txt");
    std::string caption_cmd = get_caption_cmd(argc, argv, hw_encoder, filename, encoding_flag, transcript_path);
    if (caption_cmd == "SRT_ONLY") {
        std::string remove_cmd = "rm " + output;
        res = OS::run_command(remove_cmd);
        std::cout << "Captions written to 'transcript.srt'!\n" << std::endl;
        return res.first;
    }
    std::cout << "Embedding captions..." << std::endl;
    res = OS::run_command(caption_cmd);
    if (res.first == 0) std::cout << "Captions added successfully!\n" << std::endl;
    else std::cerr << "An error occurred when adding captions.\n" << std::endl;

    res = remove_temp_file("transcript.srt");
    std::string remove_cmd = "rm " + output;
    res = OS::run_command(remove_cmd);
    return res.first;
}
