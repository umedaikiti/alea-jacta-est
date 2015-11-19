#include "shogi.h"
#include <iostream>
#include <julius/juliuslib.h>
#include <vector>
#include <cstring>
#include <iconv.h>

Shogi shogi;

int main(int argc, char* argv[])
{
	// Jconf: configuration parameters
	// load configurations from command arguments
	/*
	Jconf *jconf = j_config_load_args_new(argc, argv);
	if (jconf == NULL) {
		std::cout << "Error @ j_config_load_args_new" << std::endl;
		return -1;
	}
	*/
	jlog_set_output(NULL);
	char jconf_file1[] = "main.jconf";
	char jconf_file2[] = "am-gmm.jconf";
	Jconf *jconf = j_config_load_file_new(jconf_file1);
	if(jconf == NULL){
		std::cout << "Error @ j_config_load_file_new" << std::endl;
		return -1;
	}
	if(j_config_load_file(jconf, jconf_file2) == -1){
		std::cout << "Error @ j_config_load_file" << std::endl;
		return -1;
	}

	// Recog: Top level instance for the whole recognition process
	// create recognition instance according to the jconf
	Recog *recog = j_create_instance_from_jconf(jconf);
	if (recog == NULL) {
		std::cout << "Error @ j_create_instance_from_jconf" << std::endl;
		return -1;
	}

	// Regster callback
	callback_add(recog, CALLBACK_EVENT_SPEECH_READY, [](Recog *recog, void*) {
		std::cout << "<<< PLEASE SPEAK! >>>" << std::endl;
		shogi.print();
	}, NULL);

	callback_add(recog, CALLBACK_EVENT_SPEECH_START, [](Recog *recog, void*) {
		std::cout << "...SPEECH START..." << std::endl;
	}, NULL);

	callback_add(recog, CALLBACK_RESULT, [](Recog *recog, void*) {
		for (const RecogProcess *r = recog->process_list; r; r = r->next) {
			WORD_INFO *winfo = r->lm->winfo;
//			std::cout << r->lm->config->id << std::endl;
			switch(r->result.status){
				case 1:
					std::cout << "status : " << "recognition in progress" << std::endl;
				break;
				case 0:
					std::cout << "status : " << "recognition succeeded (at least one candidate has been found)" << std::endl;
				break;
				case -1:
					std::cout << "status : " << "search failed, no candidate has been found" << std::endl;
				break;
				case -2:
					std::cout << "status : " << "input rejected by short input" << std::endl;
				break;
				case -3:
					std::cout << "status : " << "input rejected by GMM" << std::endl;
				break;
				default:
					std::cout << "status = " << r->result.status << std::endl;
				break;
			}
			if(r->result.status != 0) continue;
			for (int n = 0; n < r->result.sentnum; ++n) {
				Sentence *s   = &(r->result.sent[n]);
				WORD_ID *seq = s->word;
				int seqnum   = s->word_num;
				char buf[512] = "";
				char buf2[512];
				char *pin = buf;
				char *pout = buf2;
				size_t ilen, rlen;
				iconv_t cd;
//				std::cout << "Score : " << s->score << std::endl;
//				std::cout << "AM Score : " << s->score_am << std::endl;
//				std::cout << "LM Score : " << s->score_lm << std::endl;
				for (int i = 1; i < seqnum-1; ++i) {
					strcat(buf, winfo->woutput[seq[i]]);//bufは十分な大きさだと考えられる
				}
				ilen = strlen(buf);
				rlen = sizeof(buf2) - 1;
				Move m;
				cd = iconv_open("UTF-8", "ISO-2022-JP");
				if(cd == (iconv_t)-1){
					std::cout << "iconv error\n";
					break;
				}
				rlen = iconv(cd, &pin, &ilen, &pout, &rlen);
				if(rlen == -1){
					std::cout << "iconv error\n";
					break;
				}
				iconv_close(cd);
				if(shogi.parse(buf2, &m)){
					shogi.move(m);
					std::cout << "success: " << buf << std::endl;
				}
				else{
					std::cout << "parse failed: " << buf << std::endl;
				}
			}
		}
	}, NULL);

	// Initialize audio input
	if (j_adin_init(recog) == FALSE) {
		return -1;
	}

	// output system information to log
	j_recog_info(recog);

	// Open input stream and recognize
	switch (j_open_stream(recog, NULL)) {
		case  0: break; // success
		case -1: std::cout << "Error in input stream" << std::endl; return -1;
		case -2: std::cout << "Failed to begin input stream" << std::endl; return -1;
	}

	// Recognition loop
	int ret = j_recognize_stream(recog);
	if (ret == -1) return -1;

	// exit
	j_close_stream(recog);
	j_recog_free(recog);

	return 0;
}
