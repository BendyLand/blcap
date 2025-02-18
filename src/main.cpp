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
#elif defined(OS_WINDOWS)
    hw_encoder = "qsv";
    encoding_flag = " -c:v h264_qsv ";
#else
    hw_encoder = "auto";
    encoding_flag = " ";
#endif
    std::unordered_set<Flag> f = parse_flags(argc, argv);
    // Due to the goto jump, these variables need to be initialized before the switch.
    std::string transcript_path;
    std::pair<int, std::string> res;
    std::string extraction_cmd;
    std::string whisper_cmd;
    std::string transcription;
	my::string temp;
    switch (f.count(Flag::COMBINE)) {
    case 1: 
        transcript_path = get_transcript_path(argc, argv);
        goto Captioning;
        break;
    default:
        break;
    }
    extraction_cmd = get_extraction_cmd(hw_encoder, filename, encoding_flag, output);
    res = OS::run_command(extraction_cmd);
    if (res.first != 0) log_err_and_exit("There was a problem extracting the transcript from the video.", res.second);

    // The whisper command is: <path>/<to>/blcap/whisper.cpp/build/bin/whisper-cli -m <path>/<to>/blcap/whisper-cpp/models/ggml-base.en.bin -f
	temp = read_file("whisper-cmd.txt");
    whisper_cmd = format_str("%s %s", temp.trim().str().c_str(), output.c_str());
    res = OS::run_command(whisper_cmd);
    if (res.first != 0) log_err_and_exit("There was a problem running Whisper.", res.second);

    std::cout << "Getting transcription..." << std::endl;
    transcription = get_transcription(res.second);
    std::cout << "Transcription:\n" << transcription << "\n" << std::endl;

    std::cout << "Converting to SRT format..." << std::endl;
    transcription = transcription_to_srt(transcription);
    std::cout << "Transcription converted successfully!\n" << std::endl;

    std::cout << "Writing SRT file..." << std::endl;
    write_file("transcript.srt", transcription);
    std::cout << "File written successfully!\n" << std::endl;

    // Absolute path where `transcript.srt` can be found. It should just be <path>/<to>/blcap/src/transcript.srt
	temp = read_file("transcript-path.txt");
    transcript_path = temp.trim();
Captioning:
    std::string caption_cmd = get_caption_cmd(argc, argv, hw_encoder, filename, encoding_flag, transcript_path);
    if (caption_cmd == "SRT_ONLY") {
        res = remove_temp_file(output);
        std::cout << "Captions written to 'transcript.srt'!\n" << std::endl;
        return res.first;
    }
    std::cout << "Embedding captions..." << std::endl;
    res = OS::run_command(caption_cmd);
    if (res.first == 0) std::cout << "Captions added successfully!\n" << std::endl;
    else {
		std::cerr << "An error occurred when adding captions.\n" << std::endl;
		return 1;
	}

    res = remove_temp_file("transcript.srt");
    res = remove_temp_file(output);

    return 0;
}
