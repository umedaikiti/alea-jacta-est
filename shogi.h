#ifndef _SHOGI_H_
#define _SHOGI_H_
#include <vector>

enum Koma {K_NULL = 0, K_HU, K_TO, K_KYOUSHA, K_NARIKYOU, K_KEIMA, K_NARIKEI, K_GIN, K_NARIGIN, K_KIN, K_GYOKU, K_HISHA, K_RYU, K_KAKU, K_UMA, K_LAST};

struct Move {
	int from_suji, from_dan;
	int to_suji, to_dan;
	int koma;	//持ち駒を使うときのみ。動かすときはK_NULL
	bool nari;
};

class Shogi
{
	int board[9][9];
	int mochigoma[2][K_LAST];//0: 先手 1: 後手 K_NULLは使わない
	int turn; //0から - 偶数: 先手の手番
	std::vector<Move> log;
private:
	int search1(int koma, const int *dx, const int *dy, int n, int to_x, int to_y, Move *m);
	int search2(int koma, const int *dx, const int *dy, int n, int to_x, int to_y, Move *m);
	int get_valid_flags(int koma, const int *dx1, const int *dy1, int n1, const int *dx2, const int *dy2, int n2, int to_x, int to_y, Move *m, int *flags, bool ryu_uma);
	int get_valid_flags_Hisha(int to_suji, int to_dan, Move *m, int *flags);
	int get_valid_flags_Kaku(int to_suji, int to_dan, Move *m, int *flags);
	int get_valid_flags_Ryu(int to_suji, int to_dan, Move *m, int *flags);
	int get_valid_flags_Uma(int to_suji, int to_dan, Move *m, int *flags);
	int get_valid_flags_Kin(int to_suji, int to_dan, int koma, Move *m, int *flags);
	int get_valid_flags_Gin(int to_suji, int to_dan, Move *m, int *flags);
	int get_valid_flags_Kyousha(int to_suji, int to_dan, Move *m, int *flags);
	int get_valid_flags_Keima(int to_suji, int to_dan, Move *m, int *flags);
//	int get_valid_flags(int koma, const int *dx1, const int *dy1, int n1, const int *dx2, const int *dy2, int n2, int to_x, int to_y, Move *m);

	bool parse_Gyoku(int to_suji, int to_dan, int flags, Move *m);
	bool parse_Hu(int to_suji, int to_dan, int flags, Move *m);
	bool parse_Kyousha(int to_suji, int to_dan, int flags, Move *m);
	bool parse_Keima(int to_suji, int to_dan, int flags, Move *m);
	bool parse_Hisha(int to_suji, int to_dan, int flags, Move *m);
	bool parse_Kaku(int to_suji, int to_dan, int flags, Move *m);
	bool parse_Ryu(int to_suji, int to_dan, int flags, Move *m);
	bool parse_Uma(int to_suji, int to_dan, int flags, Move *m);
	bool parse_Kin(int koma, int to_suji, int to_dan, int flags, Move *m);
	bool parse_Gin(int to_suji, int to_dan, int flags, Move *m);
public:
	Shogi();
	Shogi(int b[9][9], int m[2][K_LAST]);
	int getKoma(int suji, int dan){
		return board[suji-1][dan-1];
	};
	void print(void);
	void debugprint_parse(const char *str);
	void print_mochigoma(int sentegote);
	bool move(const Move &m);
	bool parse(const char *str, Move *m);
};


void set_Move(Move *m, int from_suji, int from_dan, int to_suji, int to_dan);


#define FLAG_HIDARI	0x1
#define FLAG_MIGI	0x2
#define FLAG_SUGU	0x4
#define FLAG_YORU	0x8
#define FLAG_HIKU	0x10
#define FLAG_AGARU	0x20
#define FLAG_NARI	0x40
#define FLAG_NARAZU	0x80
#define FLAG_UTU	0x100


#define FLAG_GOTE 0x100

#endif
