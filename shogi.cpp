#include <iostream>
#include <cstring>
#include <vector>
#include "shogi.h"


const char* koma_name[] = {"　", "歩", "と", "香", "杏", "桂", "圭", "銀", "全", "金", "玉", "飛", "龍", "角", "馬"};

const char* kansuji[] = {"一", "二", "三", "四", "五", "六", "七", "八", "九"};

const int default_board[9][9] = {
	{K_KYOUSHA | FLAG_GOTE, K_KEIMA | FLAG_GOTE, K_GIN | FLAG_GOTE, K_KIN | FLAG_GOTE, K_GYOKU | FLAG_GOTE, K_KIN | FLAG_GOTE, K_GIN | FLAG_GOTE, K_KEIMA | FLAG_GOTE, K_KYOUSHA | FLAG_GOTE},
	{K_NULL, K_KAKU | FLAG_GOTE, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_HISHA | FLAG_GOTE, K_NULL},
	{K_HU | FLAG_GOTE, K_HU | FLAG_GOTE, K_HU | FLAG_GOTE, K_HU | FLAG_GOTE, K_HU | FLAG_GOTE, K_HU | FLAG_GOTE, K_HU | FLAG_GOTE, K_HU | FLAG_GOTE, K_HU | FLAG_GOTE},
	{K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL},
	{K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL},
	{K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL},
	{K_HU, K_HU, K_HU, K_HU, K_HU, K_HU, K_HU, K_HU, K_HU},
	{K_NULL, K_HISHA, K_NULL, K_NULL, K_NULL, K_NULL, K_NULL, K_KAKU, K_NULL},
	{K_KYOUSHA, K_KEIMA, K_GIN, K_KIN, K_GYOKU, K_KIN, K_GIN, K_KEIMA, K_KYOUSHA}
};

//成った駒を元に戻す
static int demote(int koma)
{
	koma = koma & 0xFF;
	switch(koma){
	case K_TO:
		return K_HU;
	case K_NARIKYOU:
		return K_KYOUSHA;
	case K_NARIKEI:
		return K_KEIMA;
	case K_NARIGIN:
		return K_GIN;
	case K_RYU:
		return K_HISHA;
	case K_UMA:
		return K_KAKU;
	default:
		return koma;
	}
	
}
static int promote(int koma)
{
	int gote = koma & FLAG_GOTE;
	switch(koma & 0xFF){
	case K_HU:
		return K_TO | gote;
	case K_KYOUSHA:
		return K_NARIKYOU | gote;
	case K_KEIMA:
		return K_NARIKEI | gote;
	case K_GIN:
		return K_NARIGIN | gote;
	case K_HISHA:
		return K_RYU | gote;
	case K_KAKU:
		return K_UMA | gote;
	default:
		return koma;
	}
}


Shogi::Shogi(){
	turn = 0;
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++){
			board[i][j] = default_board[i][j];
		}
	}
	for(int i=0;i<2;i++){
		for(int j=0;j<K_LAST;j++){
			mochigoma[i][j] = 0;
		}
	}
}

Shogi::Shogi(int b[9][9], int m[2][K_LAST])
{
	turn = 0;
	for(int i=0;i<9;i++){
		for(int j=0;j<9;j++){
			board[i][j] = b[i][j];
		}
	}
	for(int i=0;i<2;i++){
		for(int j=0;j<K_LAST;j++){
			mochigoma[i][j] = m[i][j];
		}
	}
}

bool Shogi::move(const Move &m)
{
	int to_x = m.to_dan - 1;
	int to_y = m.to_suji - 1;
	if(m.koma != K_NULL){//持ち駒
		if(board[to_x][to_y] != K_NULL) return false;
		if(mochigoma[m.koma>>8][m.koma & 0xFF]>0){
			mochigoma[m.koma>>8][m.koma & 0xFF]--;
			board[to_x][to_y] = m.koma;
			turn++;
			log.push_back(m);
			return true;
		}
		else return false;
	}
	else{
		int from_x = m.from_dan - 1;
		int from_y = m.from_suji - 1;
		int koma = board[from_x][from_y];
		if(koma == K_NULL) return false;

		if(board[to_x][to_y] != K_NULL){
			int k = board[to_x][to_y];
			if((k ^ koma) & FLAG_GOTE){
				int newowner = (k & FLAG_GOTE) ? 0 : 1;
				mochigoma[newowner][demote(k)]++;
			}
			else return false;//味方の駒
		}
		if((turn & 1) == 0){
			if(m.nari)
				koma = promote(koma);
		}
		else{
			if(m.nari)
				koma = promote(koma);
		}
		board[from_x][from_y] = K_NULL;
		board[to_x][to_y] = koma;
		turn++;
		log.push_back(m);
		return true;
	}

}

void Shogi::print_mochigoma(int sentegote)
{
	for(int i=1;i<K_LAST;i++){
		for(int j=0;j<mochigoma[sentegote][i];j++){
			std::cout << koma_name[i];
		}
	}
	std::cout << std::endl;
}

void Shogi::print(void)
{
	std::cout << (turn+1) << "手目\n";
	std::cout << "持ち駒: ";
	print_mochigoma(1);
	std::cout << " ９ ８ ７ ６ ５ ４ ３ ２ １\n┏━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┓\n";
	for(int i=0;i<9;i++){
		std::cout << "┃";
		for(int j=8;j>=0;j--){
			int koma = board[i][j];
			if(koma & FLAG_GOTE){
				std::cout << "\x1b[7m" << koma_name[koma & 0xFF] << "\x1b[0m";
			}
			else{
				std::cout << koma_name[koma & 0xFF];
			}
			std::cout << "┃";
		}
		std::cout << kansuji[i] << "\n";
		if(i < 8) std::cout << "┣━━╋━━╋━━╋━━╋━━╋━━╋━━╋━━╋━━┫\n";
	}
	std::cout << "┗━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━┛" << std::endl;
	std::cout << "持ち駒: ";
	print_mochigoma(0);
}

void Shogi::debugprint_parse(const char *str)
{
	int board_old[9][9];
	Move m;
	if(!parse(str, &m)){
		std::cout << "parse failed : " << str << std::endl;
		return;
	}
	std::cout << "parse success : " << str << std::endl;
	for(int i=0;i<9;i++) for(int j=0;j<9;j++){
		board_old[i][j] = board[i][j];
	}
	move(m);
	std::cout << "持ち駒: ";
	print_mochigoma(1);
	std::cout << " ９ ８ ７ ６ ５ ４ ３ ２ １ 　 ９ ８ ７ ６ ５ ４ ３ ２ １\n┏━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┓　┏━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┳━━┓\n";
	for(int i=0;i<9;i++){
		std::cout << "┃";
		for(int j=8;j>=0;j--){
			int koma = board_old[i][j];
			if(koma & FLAG_GOTE){
				std::cout << "\x1b[7m" << koma_name[koma & 0xFF] << "\x1b[0m";
			}
			else{
				std::cout << koma_name[koma & 0xFF];
			}
			std::cout << "┃";
		}
		std::cout << kansuji[i];
		std::cout << "┃";
		for(int j=8;j>=0;j--){
			int koma = board[i][j];
			if(koma & FLAG_GOTE){
				std::cout << "\x1b[7m" << koma_name[koma & 0xFF] << "\x1b[0m";
			}
			else{
				std::cout << koma_name[koma & 0xFF];
			}
			std::cout << "┃";
		}
		std::cout << kansuji[i] << "\n";
		if(i < 8) std::cout << "┣━━╋━━╋━━╋━━╋━━╋━━╋━━╋━━╋━━┫　┣━━╋━━╋━━╋━━╋━━╋━━╋━━╋━━╋━━┫\n";
	}
	std::cout << "┗━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━┛　┗━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━┻━━┛" << std::endl;
	std::cout << "持ち駒: ";
	print_mochigoma(0);
}

//result[0] : a[i] < bとなるiの数
//result[1] : a[i] == b
//result[2] : a[i] > b
static void compare(int *a, int b, int n, int result[3])
{
	result[0] = result[1] = result[2] = 0;
	for(int i=0;i<n;i++){
		if(a[i] < b) result[0]++;
		else if(a[i] == b) result[1]++;
		else result[2]++;
	}
}

static bool can_promote(const Move &m, bool gote, int koma)
{
	switch(koma & 0xFF){
	case K_HU:
	case K_KYOUSHA:
	case K_KEIMA:
	case K_GIN:
	case K_HISHA:
	case K_KAKU:
		if(gote){
			return m.to_dan >= 7 || m.from_dan >= 7;
		}
		else{
			return m.to_dan <= 3 || m.from_dan <= 3;
		}
	default:
		return false;
	}
}

//n:dx, dyの要素数
//mはMoveのn個以上の配列
//dx, dyは駒の動き
//goteは後手の手番か - dxの符号を反転する
//香車、飛車、角などのどこまでも進める駒の動き
//Moveのnariはセットされない
int Shogi::search1(int koma, const int *dx, const int *dy, int n, int to_x, int to_y, Move *m)
{
	bool gote = ((koma & FLAG_GOTE) != 0);
	int reverse_dx = (gote ? -1 : 1);
	int count = 0;
	Move m_default;
	m_default.to_dan = to_x + 1;
	m_default.to_suji = to_y + 1;
	m_default.koma = K_NULL;
	m_default.nari = false;
	for(int i=0;i<n;i++){
		int x = to_x - dx[i] * reverse_dx;
		int y = to_y - dy[i];
		while(0 <= x && x < 9 && 0 <= y && y < 9 && board[x][y] == K_NULL){
			x -= dx[i] * reverse_dx;
			y -= dy[i];
		}
		if(0 <= x && x < 9 && 0 <= y && y < 9 && board[x][y] == koma){
			m[count] = m_default;
			m[count].from_dan = x + 1;
			m[count].from_suji = y + 1;
			count++;
		}
	}
	return count;
}

int Shogi::search2(int koma, const int *dx, const int *dy, int n, int to_x, int to_y, Move *m)
{
	int count = 0;
	int reverse_dx = (((koma & FLAG_GOTE) != 0) ? -1 : 1);
	Move m_default;
	m_default.to_dan = to_x + 1;
	m_default.to_suji = to_y + 1;
	m_default.koma = K_NULL;
	m_default.nari = false;
	for(int i=0;i<n;i++){
		int x = to_x - dx[i] * reverse_dx;
		int y = to_y - dy[i];
		if(board[x][y] == koma){
			m[count] = m_default;
			m[count].from_dan = x + 1;
			m[count].from_suji = y + 1;
			count++;
		}
	}
	return count;
}

//to - from
#define XPLUS	0x01
#define XZERO	0x02
#define XMINUS	0x04
#define XMASK	(XPLUS | XZERO | XMINUS)
#define YPLUS	0x08
#define YZERO	0x10
#define YMINUS	0x20
#define YMASK	(YPLUS | YZERO | YMINUS)

int Shogi::get_valid_flags(int koma, const int *dx1, const int *dy1, int n1, const int *dx2, const int *dy2, int n2, int to_x, int to_y, Move *m, int *flags, bool ryu_uma)
{
	bool gote = ((koma & FLAG_GOTE) != 0);
	int count = 0;
	Move tmp[16];//十分大きい
	int tmpflags[16];
	int f[16];
	int n;
	n = search1(koma, dx1, dy1, n1, to_x, to_y, tmp);
	n += search2(koma, dx2, dy2, n2, to_x, to_y, tmp+n);
	if(n == 0){
		if(mochigoma[gote ? 1 : 0][koma & 0xFF] == 0){
			return 0;
		}
		m[0].to_dan = to_x + 1;
		m[0].to_suji = to_y + 1;
		m[0].nari = false;
		m[0].koma = koma;
		m[0].from_dan = 0;
		m[0].from_suji = 0;
		flags[0] = 0;
		return 1;
	}
	else if(n == 1){
		tmpflags[0] = 0;
	}
	else{
		for(int i=0;i<n;i++){
			f[i] = 0;
			int from_x = tmp[i].from_dan - 1;
			int from_y = tmp[i].from_suji - 1;
			if(from_x < to_x) f[i] |= XPLUS;
			else if(from_x == to_x) f[i] |= XZERO;
			else f[i] |= XMINUS;
			if(from_y < to_y) f[i] |= YPLUS;
			else if(from_y == to_y) f[i] |= YZERO;
			else f[i] |= YMINUS;
		}
		int xplus = 0, xzero = 0, xminus = 0;
		int yplus = 0, yzero = 0, yminus = 0;
		for(int i=0;i<n;i++){
			if(f[i] & XPLUS){
				xplus++;
			}
			if(f[i] & XZERO){
				xzero++;
			}
			if(f[i] & XMINUS){
				xminus++;
			}
			if(f[i] & YPLUS){
				yplus++;
			}
			if(f[i] & YZERO){
				yzero++;
			}
			if(f[i] & YMINUS){
				yminus++;
			}
		}

		for(int i=0;i<n;i++){
			if(f[i] & XPLUS){
				if(xplus == 1) f[i] &= ~YMASK;
			}
			if(f[i] & XZERO){
				if(xzero == 1) f[i] &= ~YMASK;
			}
			if(f[i] & XMINUS){
				if(xminus == 1) f[i] &= ~YMASK;
			}
		}
	
		for(int i=0;i<n;i++) if(f[i] & YMASK){
			if(f[i] & YPLUS){
				if(yplus == 1) f[i] &= ~XMASK;
			}
			if(f[i] & YZERO){
				if(yzero == 1) f[i] &= ~XMASK;
			}
			if(f[i] & YMINUS){
				if(yminus == 1) f[i] &= ~XMASK;
			}
		}
	
		if(ryu_uma){
			if(n != 2) return -1;//ありえないはず
			int i = 0;
			for(;i<2;i++){
				if(f[i] & YZERO) break;
			}
			if(i<2){
				if(f[1-i] & YPLUS){
					f[i] = YMINUS;
					f[1-i] = YPLUS;
				}
				else{
					f[i] = YPLUS;
					f[1-i] = YMINUS;
				}
			}
		}
		else{
			for(int i=0;i<n;i++){
				if(f[i] & YZERO) f[i] = YZERO;
			}
		}
	
		if(gote){
			for(int i=0;i<n;i++){
				tmpflags[i] = 0;
				if(f[i] & XPLUS) tmpflags[i] |= FLAG_AGARU;
				if(f[i] & XZERO) tmpflags[i] |= FLAG_YORU;
				if(f[i] & XMINUS) tmpflags[i] |= FLAG_HIKU;
				if(f[i] & YPLUS) tmpflags[i] |= FLAG_HIDARI;
				if(f[i] & YZERO) tmpflags[i] |= FLAG_SUGU;
				if(f[i] & YMINUS) tmpflags[i] |= FLAG_MIGI;
			}
		}
		else{
			for(int i=0;i<n;i++){
				tmpflags[i] = 0;
				if(f[i] & XPLUS) tmpflags[i] |= FLAG_HIKU;
				if(f[i] & XZERO) tmpflags[i] |= FLAG_YORU;
				if(f[i] & XMINUS) tmpflags[i] |= FLAG_AGARU;
				if(f[i] & YPLUS) tmpflags[i] |= FLAG_MIGI;
				if(f[i] & YZERO) tmpflags[i] |= FLAG_SUGU;
				if(f[i] & YMINUS) tmpflags[i] |= FLAG_HIDARI;
			}
		}
	}

	for(int i=0;i<n;i++){
		m[count] = tmp[i];
		flags[count] = tmpflags[i];
		count++;
		if(can_promote(tmp[i], gote, koma)){
			m[count] = tmp[i];
			m[count].nari = true;
			flags[count] = tmpflags[i] | FLAG_NARI;
			flags[count-1] |= FLAG_NARAZU;
			count++;
		}
	}

	if(mochigoma[gote ? 1 : 0][koma & 0xFF] > 0){
		m[count].to_dan = to_x + 1;
		m[count].to_suji = to_y + 1;
		m[count].nari = false;
		m[count].koma = koma;
		m[count].from_dan = 0;
		m[count].from_suji = 0;
		flags[count] = FLAG_UTU;
		count++;
	}
	return count;
}

static const int naname_dx[] = {1, 1, -1, -1};
static const int naname_dy[] = {-1, 1, -1, 1};
static const int naname_n = 4;
static const int tateyoko_dx[] = {1, 0, -1, 0};
static const int tateyoko_dy[] = {0, 1, 0, -1};
static const int tateyoko_n = 4;

int Shogi::get_valid_flags_Hisha(int to_suji, int to_dan, Move *m, int *flags)
{
	int koma = K_HISHA;
	if(turn & 1) koma |= FLAG_GOTE;
	return get_valid_flags(koma, tateyoko_dx, tateyoko_dy, tateyoko_n, NULL, NULL, 0, to_dan-1, to_suji-1, m, flags, true);
}

int Shogi::get_valid_flags_Kaku(int to_suji, int to_dan, Move *m, int *flags)
{
	int koma = K_KAKU;
	if(turn & 1) koma |= FLAG_GOTE;
	return get_valid_flags(koma, naname_dx, naname_dy, naname_n, NULL, NULL, 0, to_dan-1, to_suji-1, m, flags, true);
}

int Shogi::get_valid_flags_Ryu(int to_suji, int to_dan, Move *m, int*flags)
{
	int koma = K_RYU;
	if(turn & 1) koma |= FLAG_GOTE;
	return get_valid_flags(koma, tateyoko_dx, tateyoko_dy, tateyoko_n, naname_dx, naname_dy, naname_n, to_dan-1, to_suji-1, m, flags, true);
}

int Shogi::get_valid_flags_Uma(int to_suji, int to_dan, Move *m, int*flags)
{
	int koma = K_UMA;
	if(turn & 1) koma |= FLAG_GOTE;
	return get_valid_flags(koma, naname_dx, naname_dy, naname_n, tateyoko_dx, tateyoko_dy, tateyoko_n, to_dan-1, to_suji-1, m, flags, true);
}

static const int kin_dx[] = {-1, -1, -1, 0, 0, 1};
static const int kin_dy[] = {-1, 0, 1, -1, 1, 0};
static const int kin_n = 6;

int Shogi::get_valid_flags_Kin(int to_suji, int to_dan, int koma, Move *m, int *flags)
{
	return get_valid_flags(koma, NULL, NULL, 0, kin_dx, kin_dy, kin_n, to_dan-1, to_suji-1, m, flags, false);
}

static const int gin_dx[] = {-1, -1, -1, 1, 1};
static const int gin_dy[] = {-1, 0, 1, -1, 1};
static const int gin_n = 5;

int Shogi::get_valid_flags_Gin(int to_suji, int to_dan, Move *m, int *flags)
{
	int koma = K_GIN;
	if(turn & 1) koma |= FLAG_GOTE;
	return get_valid_flags(koma, NULL, NULL, 0, gin_dx, gin_dy, gin_n, to_dan-1, to_suji-1, m, flags, false);
}

static const int kyousha_dx[] = {-1};
static const int kyousha_dy[] = {0};
static const int kyousha_n = 1;

int Shogi::get_valid_flags_Kyousha(int to_suji, int to_dan, Move *m, int *flags)
{
	int koma = K_KYOUSHA;
	if(turn & 1) koma |= FLAG_GOTE;
	return get_valid_flags(koma, kyousha_dx, kyousha_dy, kyousha_n, NULL, NULL, 0, to_dan-1, to_suji-1, m, flags, false);
}

static const int keima_dx[] = {-2, -2};
static const int keima_dy[] = {-1, 1};
static const int keima_n = 2;

int Shogi::get_valid_flags_Keima(int to_suji, int to_dan, Move *m, int *flags)
{
	int koma = K_KEIMA;
	if(turn & 1) koma |= FLAG_GOTE;
	return get_valid_flags(koma, NULL, NULL, 0, keima_dx, keima_dy, keima_n, to_dan-1, to_suji-1, m, flags, false);
}

static const int gyoku_suji[] = {1, 1, 1, 0, 0, -1, -1, -1};
static const int gyoku_dan[] = {-1, 0, 1, -1, 1, -1, 0, 1};

static const char *flag_str[] = {"左", "右", "直", "寄", "引", "上", "成", "不成", "打"};

bool Shogi::parse(const char *str, Move *m)
{
	int cl;
	int to_suji, to_dan;
	cl = strlen("同");
	if(strncmp("同", str, cl) == 0){
		str += cl;
		if(log.empty()) return false;
		int last = log.size() - 1;
		to_suji = log[last].to_suji;
		to_dan = log[last].to_dan;
	}
	else{
		to_suji = str[0] - '0';
		to_dan = str[1] - '0';
		if(to_suji <= 0 || 9 < to_suji) return false;
		if(to_dan <= 0 || 9 < to_dan) return false;
		str += 2;
	}
//駒の名前をparse
	int koma = K_NULL;
	for(int i=1;i<K_LAST;i++){
		cl = strlen(koma_name[i]);
		if(strncmp(koma_name[i], str, cl) == 0){
			koma = i;
			str += cl;
			break;
		}
	}
	if(koma == K_NULL) return false;

	int flags = 0;

	for(int i=0;i<9;i++){
		cl = strlen(flag_str[i]);
		if(strncmp(flag_str[i], str, cl) == 0){
			flags |= 1 << i;
			str += cl;
		}
	}

	int from_suji, from_dan;
	switch(koma){
	case K_GYOKU:
		return parse_Gyoku(to_suji, to_dan, flags, m);
	case K_HU:
		return parse_Hu(to_suji, to_dan, flags, m);
	case K_KYOUSHA:
		return parse_Kyousha(to_suji, to_dan, flags, m);
	case K_KEIMA:
		return parse_Keima(to_suji, to_dan, flags, m);
	case K_GIN:
		return parse_Gin(to_suji, to_dan, flags, m);
	case K_KIN:
	case K_NARIGIN:
	case K_NARIKYOU:
	case K_NARIKEI:
	case K_TO:
		return parse_Kin(koma | ((turn & 1) ? FLAG_GOTE : 0), to_suji, to_dan, flags, m);
	case K_HISHA:
		return parse_Hisha(to_suji, to_dan, flags, m);
	case K_KAKU:
		return parse_Kaku(to_suji, to_dan, flags, m);
	case K_UMA:
		return parse_Uma(to_suji, to_dan, flags, m);
	case K_RYU:
		return parse_Ryu(to_suji, to_dan, flags, m);
	}
	std::cout << str << std::endl;
	return false;
}

void set_Move(Move *m, int from_suji, int from_dan, int to_suji, int to_dan)
{
	m->from_suji = from_suji;
	m->from_dan = from_dan;
	m->to_suji = to_suji;
	m->to_dan = to_dan;
}


bool Shogi::parse_Hu(int to_suji, int to_dan, int flags, Move *m)
{
	int from_suji, from_dan;
	int koma = K_HU;
	bool onboard = false;
	if(turn & 1) koma |= FLAG_GOTE;
	from_suji = to_suji;
	if(turn & 1){
		if(to_dan == 9 && ((flags & FLAG_NARI) == 0)) return false;
		from_dan = to_dan - 1;
		if(from_dan > 1 && board[from_dan-1][from_suji-1] == koma){
			//成、不成を指定してない
			if(to_dan >= 7 && (flags & (FLAG_NARI | FLAG_NARAZU)) == 0) return false;
			//駒の移動
			onboard = true;
		}
	}
	else{
		if(to_dan == 1 && ((flags & FLAG_NARI) == 0)) return false;
		from_dan = to_dan + 1;
		if(from_dan <= 9 && board[from_dan-1][from_suji-1] == koma){
			//成、不成を指定してない
			if(to_dan <= 3 && (flags & (FLAG_NARI | FLAG_NARAZU)) == 0) return false;
			onboard = true;
		}
	}
	if(onboard){
		m->from_dan = from_dan;
		m->from_suji = from_suji;
		m->to_dan = to_dan;
		m->to_suji = to_suji;
		m->koma = K_NULL;
		m->nari = (flags & FLAG_NARI) != 0;
		return true;
	}
	//持ち駒あるか
	if(mochigoma[turn & 1][K_HU] == 0) return false;
	//持ち駒ある
	for(int i=0;i<9;i++){
		if(board[i][to_suji-1] == koma) return false;//二歩
	}
	
	m->from_dan = 0;
	m->from_suji = 0;
	m->to_dan = to_dan;
	m->to_suji = to_suji;
	m->koma = koma;
	m->nari = false;
	return true;
}

bool Shogi::parse_Hisha(int to_suji, int to_dan, int flags, Move *m)
{
	Move tmp[4];
	int cflags[4];
	int n = get_valid_flags_Hisha(to_suji, to_dan, tmp, cflags);
	if(n == 0) return false;
	for(int i=0;i<n;i++){
		if(cflags[i] == flags){
			*m = tmp[i];
			return true;
		}
	}
	return false;
}

bool Shogi::parse_Kaku(int to_suji, int to_dan, int flags, Move *m)
{
	Move tmp[32];
	int cflags[32];
	int n = get_valid_flags_Kaku(to_suji, to_dan, tmp, cflags);
	if(n == 0) return false;
	for(int i=0;i<n;i++){
		if(cflags[i] == flags){
			*m = tmp[i];
			return true;
		}
	}
	return false;
}


bool Shogi::parse_Ryu(int to_suji, int to_dan, int flags, Move *m)
{
	Move tmp[32];
	int cflags[32];
	int n = get_valid_flags_Ryu(to_suji, to_dan, tmp, cflags);
	if(n == 0) return false;
	for(int i=0;i<n;i++){
		if(cflags[i] == flags){
			*m = tmp[i];
			return true;
		}
	}
	return false;
}

bool Shogi::parse_Uma(int to_suji, int to_dan, int flags, Move *m)
{
	Move tmp[32];
	int cflags[32];
	int n = get_valid_flags_Uma(to_suji, to_dan, tmp, cflags);
	if(n == 0) return false;
	for(int i=0;i<n;i++){
		if(cflags[i] == flags){
			*m = tmp[i];
			return true;
		}
	}
	return false;
}

bool Shogi::parse_Kin(int koma, int to_suji, int to_dan, int flags, Move *m)
{
	Move tmp[32];
	int cflags[32];
	int n = get_valid_flags_Kin(to_suji, to_dan, koma, tmp, cflags);
	if(n == 0) return false;
	for(int i=0;i<n;i++){
		if(cflags[i] == flags){
			*m = tmp[i];
			return true;
		}
	}
	return false;
}

bool Shogi::parse_Gin(int to_suji, int to_dan, int flags, Move *m)
{
	Move tmp[32];
	int cflags[32];
	int n = get_valid_flags_Gin(to_suji, to_dan, tmp, cflags);
	if(n == 0) return false;
	for(int i=0;i<n;i++){
		if(cflags[i] == flags){
			*m = tmp[i];
			return true;
		}
	}
	return false;
}

bool Shogi::parse_Gyoku(int to_suji, int to_dan, int flags, Move *m)
{
	bool found = false;
	int from_dan, from_suji;
	int gyoku = K_GYOKU;
	if(turn & 1) gyoku |= FLAG_GOTE;
	for(int i=0;i<8;i++){
		from_dan = to_dan + gyoku_dan[i];
		from_suji = to_suji + gyoku_suji[i];
		if(from_dan < 1 || from_suji > 9){
			continue;
		}
		if(board[from_dan-1][from_suji-1] == gyoku){
			found = true;
			break;
		}
	}
	if(!found) return false;
	m->from_dan = from_dan;
	m->from_suji = from_suji;
	m->to_dan = to_dan;
	m->to_suji = to_suji;
	m->koma = K_NULL;
	m->nari = false;
	return true;
}

bool Shogi::parse_Kyousha(int to_suji, int to_dan, int flags, Move *m)
{
	Move tmp[16];
	int cflags[16];
	//get_valid_flags_Kyoushaでは除かれない
	if(turn & 1){
		if(to_dan == 9 && (flags & (FLAG_NARI | FLAG_NARAZU)) != FLAG_NARI)
			return false;
	}
	else{
		if(to_dan == 1 && (flags & (FLAG_NARI | FLAG_NARAZU)) != FLAG_NARI)
			return false;
	}
	int n = get_valid_flags_Kyousha(to_suji, to_dan, tmp, cflags);
	if(n == 0) return false;
	for(int i=0;i<n;i++){
		if(cflags[i] == flags){
			*m = tmp[i];
			return true;
		}
	}
	return false;
}
bool Shogi::parse_Keima(int to_suji, int to_dan, int flags, Move *m)
{
	Move tmp[16];
	int cflags[16];
	if(turn & 1){
		if(to_dan >= 8 && (flags & (FLAG_NARI | FLAG_NARAZU)) != FLAG_NARI)
			return false;
	}
	else{
		if(to_dan <= 2 && (flags & (FLAG_NARI | FLAG_NARAZU)) != FLAG_NARI)
			return false;
	}
	int n = get_valid_flags_Keima(to_suji, to_dan, tmp, cflags);
	if(n == 0) return false;
	for(int i=0;i<n;i++){
		if(cflags[i] == flags){
			*m = tmp[i];
			return true;
		}
	}
	return false;
}
