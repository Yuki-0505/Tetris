#include<graphics.h>
#include<time.h>
#include<stdlib.h>
#include<stdio.h>
#include<conio.h>
#include<mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define mapWidth 13
#define mapHeight 20
#define picWidth 36
#define picHeight 36
#define textWidth 200
#define textHeight 64
#define numWidth 24
#define numHeight 36
#define screenWidth mapWidth*picWidth
#define screenHeight  mapHeight*picWidth
#define KEY_DOWN(VK_NONAME)(GetAsyncKeyState(VK_NONAME) ? 1 : 0)
//全局变量
bool regame = false;
int last = 0, Time = -350;
//音乐类
class Music {
public:
	char load[3][30];
	char music[3][30];
	char close[3][30];
	Music() {
		for (int i = 0; i < 3; i++) {
			sprintf_s(load[i], "open ./audio/m%d.mp3", i);
			sprintf_s(music[i], "play ./audio/m%d.mp3", i);
			sprintf_s(close[i], "close ./audio/m%d.mp3", i);
		}
	}
	void play(int n) {
		mciSendString(close[last], NULL, 0, 0);
		mciSendString(load[n], NULL, 0, 0);
		mciSendString(music[n], NULL, 0, 0);
		last = n;
	}
};
//分数类
class Score {
public:
	IMAGE num[10];
	int score = 0, best[4] = { 0 };
	Score() {
		//加载图片
		char str[30] = {};
		for (int i = 0; i < 10; i++) {
			sprintf_s(str, "./image/number_%d.bmp", i);
			loadimage(&num[i], str, numWidth, numHeight);
		}
		//读取文件
		FILE *file;
		if (file = fopen("./document/score.txt", "rb+")) {
			for (int i = 0; i < 3; i++) {
				fread(&best[i], sizeof(best[i]), 1, file);
			}
		}
		fclose(file);
	}
	//得分
	void get(int score) {
		this->score += score;
	}
	//排序
	void sort() {
		for (int i = 3; i > 0; i--) {
			if (best[i] > best[i - 1]) {
				int tmp = best[i];
				best[i] = best[i - 1];
				best[i - 1] = tmp;
			}
		}
	}
	//绘画分数
	void show(int width = screenWidth - 2 * numWidth, int height = numHeight / 2, int flag = 4) {
		int n, tmp = best[flag];
		if (flag == 4) {
			tmp = score;
		}
		//数字转图片
		for (int i = 0;; i++) {
			n = tmp % 10;
			tmp /= 10;
			putimage(width - i * numWidth, height, &num[n], SRCAND);
			if (tmp == 0) {
				break;
			}
		}
	}
};
//界面类
class Interface {
public:
	IMAGE text[3], space, mine;
	MOUSEMSG mmsg;
	bool flag = true;
	int times, speed[3] = { 200, 125, 50 };
	char text_info[5][15] = {
		{"初级"},
		{"中级"},
		{"高级"},
		{"按E键继续..."},
		{"按Q键退出..."}
	};
	Interface() {
		loadimage(&text[0], "./image/text_ready.bmp", textWidth, textHeight);
		loadimage(&text[1], "./image/text_over.bmp", textWidth, textHeight);
		loadimage(&text[2], "./image/overface.bmp", textWidth * 2, textWidth);
		loadimage(&space, "./image/Space.bmp", 6 * picWidth, 6 * picHeight);
		loadimage(&mine, "./image/mine.jpg", picWidth, picHeight * 16);
	}
	//排行榜界面
	bool ranking_list(Score &score) {
		//判断是否有鼠标消息
		if (MouseHit()) {
			mmsg = GetMouseMsg();
		}
		//缓冲打印
		BeginBatchDraw();
		if (screenWidth + picWidth < mmsg.x&&mmsg.x < screenWidth + textWidth
			&& 12 * picHeight < mmsg.y&&mmsg.y < 13 * picHeight) {
			//空白覆盖、排名、分数、变色、标记
			putimage(screenWidth + picWidth, 13 * picHeight + 2, &space);
			for (int i = 0; i < 3; i++) {
				score.show(screenWidth + 5 * picWidth, (14 + 2 * i) * picHeight, i);
				putimage(screenWidth + picWidth, (14 + 2 * i) * picHeight, picWidth, picHeight, &mine, 0, (14 - i)*picHeight);
			}
			setcolor(RGB(0, 255, 0));
			flag = true;
		}
		else {
			//标记生效空白覆盖、去标记、变色
			if (flag) {
				putimage(screenWidth + picWidth, 13 * picHeight + 2, &space);
				flag = false;
			}
			setcolor(RGB(255, 0, 255));
		}
		//打印方框、字体
		rectangle(screenWidth + picWidth, 12 * picHeight, screenWidth + textWidth, 13 * picHeight);
		outtextxy(screenWidth + 1.15*picWidth, 12.15 * picHeight, " 排  行  榜 ");
		EndBatchDraw();
		return flag;
	}
	//开始界面
	void gameBegin(Score &score) {
		//getready
		putimage(screenWidth + mapWidth, picHeight, &text[0], SRCAND);
		while (true) {
			//'Q'键
			if (KEY_DOWN('Q')) {
				exit(0);
			}
			if (ranking_list(score)) {
				continue;
			}
			//鼠标信息获取
			if (MouseHit()) {
				mmsg = GetMouseMsg();
			}
			//难度选择字体打印
			for (int i = 0; i < 3; i++) {
				//颜色改变
				setcolor(RGB(255, 0, 255));
				if (mmsg.x > screenWidth + picWidth && mmsg.x < screenWidth + textWidth &&
					mmsg.y >(14 + 2 * i) * picHeight && mmsg.y < (15 + 2 * i) * picHeight) {
					setcolor(RGB(0, 255, 0));
					if (mmsg.uMsg == WM_LBUTTONDOWN) {
						times = speed[i];
						setcolor(RGB(255, 0, 255));
						return;
					}
				}
				//字体、方框
				outtextxy(screenWidth + 2.5*picWidth, (14.15 + 2 * i)*picHeight, text_info[i]);
				rectangle(screenWidth + picWidth, (14 + 2 * i) * picHeight, screenWidth + textWidth, (15 + 2 * i) * picHeight);
			}
		}
	}
	//结束界面
	void gameOver(Score &score) {
		//计算最高分
		score.best[3] = score.score;
		score.sort();
		//打印gameover、overface
		putimage(screenWidth + mapWidth, picHeight, &text[1], SRCAND);
		putimage(screenWidth / 2 - textWidth, screenHeight / 2 - textWidth / 2, &text[2]);
		//分数图片、提示
		for (int i = 0; i < 2; i++) {
			score.show((5 * i + 4.5)* picWidth, (screenHeight - numHeight) / 2, 4 * i);
			outtextxy(screenWidth / 2 - 2 * picWidth, screenHeight / 2 + (i + 0.5)*picHeight, text_info[i + 3]);
		}
		//存入文件
		FILE *file;
		if (file = fopen("./document/score.txt", "wb+")) {
			for (int i = 0; i < 3; i++) {
				fwrite(&score.best[i], sizeof(score.best[i]), 1, file);
			}
		}
		fclose(file);
		//'E','Q'键
		while (!KEY_DOWN('E')) {
			if (KEY_DOWN('Q')) {
				exit(0);
			}
		}
	}
};
//方块类
class Square {
public:
	IMAGE img, pic, mine;
	int color;//方块颜色
	int state;//当前方块样式
	int next;//下一方块样式
	int square[4][2];//4个小方块坐标
	int tmp[4][2];//预测的4个小方块坐标
	//方块的8种样式
	int style[8][4][2] = {
		{{-1, 0},{ 0, 0},{ 1, 0},{ 2, 0}},
		{{-1, 0},{ 0, 0},{ 1, 0},{ 1, 1}},
		{{-1, 0},{ 0, 0},{ 1, 0},{ 1,-1}},
		{{-1, 0},{ 0, 0},{ 0, 1},{-1, 1}},
		{{ 1, 0},{ 0, 0},{ 0, 1},{-1, 1}},
		{{-1, 0},{ 0, 0},{ 1, 0},{ 0, 1}},
		{{-1, 0},{ 0, 0},{ 0, 1},{ 1, 1}},
		{{ 0, 0}}
	};
	Square() {
		next = rand() % 7;
		birth();
		loadimage(&img, "./image/Color.bmp", picWidth * 9, picHeight);
		loadimage(&pic, "./image/Square.bmp", picWidth * 4, picHeight * 14);
		loadimage(&mine, "./image/mine.jpg", picWidth, picHeight * 16);
	}
	//随机生成方块
	void birth() {
		state = next;
		next = rand() % 8;
		for (int i = 0; i < 4; i++) {
			square[i][0] = tmp[i][0] = style[state][i][0] + mapWidth / 2;
			square[i][1] = tmp[i][1] = style[state][i][1];
		}
	}
	//旋转（核心）
	void rotate() {
		//正方形不旋转
		if (state == 3 || state == 7) {
			return;
		}
		//获取中心点
		int x = square[1][0], y = square[1][1];
		//旋转
		for (int i = 0; i < 4; i++) {
			tmp[i][0] = -(square[i][1] - y) + x;
			tmp[i][1] = square[i][0] - x + y;
		}
	}
	//移动（预测）
	void move(int vec) {
		for (int i = 0; i < 4; i++) {
			tmp[i][0] = square[i][0] + vec;
		}
	}
	//下落（预测）
	void down() {
		for (int i = 0; i < 4; i++) {
			tmp[i][1] = square[i][1] + 1;
		}
	}
	//预测点不可到达，回退
	void sync() {
		for (int i = 0; i < 4; i++) {
			tmp[i][0] = square[i][0];
			tmp[i][1] = square[i][1];
		}
	}
	//预测点可到达，实际点同步预测点
	void arrive() {
		for (int i = 0; i < 4; i++) {
			square[i][0] = tmp[i][0];
			square[i][1] = tmp[i][1];
		}
	}
	//定点贴图
	void showxy(int i, int j) {
		putimage(j*picWidth, i*picHeight, picWidth, picHeight, &img, color*picWidth, 0);
	}
	//打印方块
	void show() {
		//小点
		if (state == 7) {
			putimage(square[0][0] * picWidth, square[0][1] * picHeight, picWidth, picHeight, &mine, 0, (rand() % 2 * 2 + 3)*picHeight);
			return;
		}
		//方块
		color = rand() % 9;
		for (int i = 0; i < 4; i++) {
			showxy(square[i][1], square[i][0]);
		}
	}
	//打印下一个方块
	void show_next() {
		//下一个为小点
		if (next == 7) {
			putimage(screenWidth + 2 * picWidth, picHeight, picWidth, picHeight, &mine, 0, 3 * picHeight);
			return;
		}
		//方块
		putimage(screenWidth + picWidth, picHeight, picWidth * 4, picHeight * 2, &pic, 0, 2 * next*picHeight, SRCAND);
	}
};
//地图类
class Map {
public:
	IMAGE img, bg;
	Music music;
	Score score;
	Interface intfc;
	int map[mapHeight + 1][mapWidth + 1], color, rate = 0;
	int flag[mapHeight + 1][mapWidth + 1];
	char hint_info[8][20] = {
		{"W : 变形"},
		{"S : 加速"},
		{"A : 左移"},
		{"D : 右移"},
		{"F : 暂停"},
		{"E : 继续"},
		{"R : 重来"},
		{"Q : 退出"},
	};
	Map() {
		loadimage(&img, "./image/Color.bmp", picWidth * 9, picHeight);
		loadimage(&bg, "./image/bilibili.bmp", screenWidth + 6 * picWidth, screenHeight);
		for (int i = 0; i < mapWidth; i++) {
			map[mapHeight][i] = 1;
		}
		cleardevice();
		show();
		intfc.gameBegin(score);
	}
	//向上移动
	void move_up() {
		//频率
		if ((++rate %= 8) != 0) {
			return;
		}
		//移动
		for (int i = 0; i < mapHeight; i++) {
			for (int j = 0; j < mapWidth; j++) {
				map[i][j] = map[i + 1][j];
			}
		}
		//设置空点
		map[mapHeight - 1][rand() % mapWidth] = 0;
		map[mapHeight - 1][rand() % mapWidth] = 0;
	}
	//消行
	void remove(int row) {
		//播放音乐1
		music.play(1);
		bar(0, row*picHeight, screenWidth, (row + 1)*picHeight);
		//计算分数
		int num = 0;
		for (int j = 0; j < mapWidth; j++) {
			num += map[row][j];
		}
		//加分
		score.get(num);
		//下移
		for (int i = row; i > 0; i--) {
			for (int j = 0; j < mapWidth; j++) {
				map[i][j] = map[i - 1][j];
			}
		}
		//0行归零
		for (int j = 0; j < mapWidth; j++) {
			map[0][j] = 0;
		}
	}
	//满行检测
	void test() {
		for (int i = 0, j; i < mapHeight; i++) {
			for (j = 0; j < mapWidth; j++) {
				//检测到空点退出
				if (map[i][j] == 0) {
					break;
				}
			}
			//消行
			if (j == mapWidth) {
				remove(i);
			}
		}
	}
	//叠加
	void join(Square &square) {
		//播放音乐2
		music.play(2);
		//炸弹检测
		if (square.state == 7) {
			remove(square.square[0][1]);
			return;
		}
		//碰顶检测
		for (int i = 0; i < 4; i++) {
			//结束
			if (square.square[i][1] <= 0) {
				//播放音乐0
				music.play(0);
				regame = true;
				return;
			}
			//动态方块转静态
			map[square.square[i][1]][square.square[i][0]] = 1;
		}
		//上至下逐行检测消除
		test();
		//固定速率上移地图
		move_up();
	}
	//定点贴图
	void showxy(int i, int j) {
		if (map[i][j] == 0) {
			return;
		}
		if (color % 4 == 0) {
			flag[i][j] = rand() % 9;
		}
		//color = rand() % 9;
		putimage(j*picWidth, i*picHeight, picWidth, picHeight, &img, flag[i][j] * picWidth, 0);
	}
	//绘画全图
	void show() {
		//贴图
		putimage(0, 0, &bg);
		color++;
		for (int i = 0; i < mapHeight; i++) {
			line(0, i*picHeight, screenWidth, i*picHeight);
			for (int j = 0; j < mapWidth; j++) {
				showxy(i, j);
			}
		}
		//绘画网格线
		for (int i = 0; i <= mapWidth; i++) {
			line(i*picWidth, 0, i*picWidth, screenHeight);
		}
		//提示信息
		for (int i = 0; i < 8; i++) {
			outtextxy(screenWidth + picWidth, (i + 4)*picHeight, hint_info[i]);
		}
	}
};
//是否重来游戏
void ReGame(Map &map, Square &square) {
	//结束界面
	if (regame) {
		map.intfc.gameOver(map.score);
	}
	//下一方块
	else {
		square.show_next();
	}
}
//按键检测
bool Key_Down(Square &square, Map &map, int &T, int &times) {
	//计算时间间隔
	times = map.intfc.times - 4 * map.score.score / mapWidth;
	//标记设为true
	bool flag = true;
	//加速
	if (KEY_DOWN('S')) {
		times = 25;
	}
	//重来
	else if (KEY_DOWN('R')) {
		regame = true;
	}
	//退出
	else if (KEY_DOWN('Q')) {
		exit(0);
	}
	//暂停
	else if (KEY_DOWN('F')) {
		square.show_next();
		while (!KEY_DOWN('E')) {
			if (KEY_DOWN('Q')) {
				exit(0);
			}
		}
	}
	//下落
	if ((++T %= 3) == 0) {
		square.down();
		return flag;
	}
	//左
	if (KEY_DOWN('A')) {
		square.move(-1);
		flag = false;
		times = 100;
	}
	//右
	else if (KEY_DOWN('D')) {
		square.move(1);
		flag = false;
		times = 100;
	}
	//变形
	else if (KEY_DOWN('W')) {
		square.rotate();
		flag = false;
		times = 150;
	}
	//返回flag
	return flag;
}
//碰撞检测
void Carsh(Map &myMap, Square &square, bool flag) {
	int i;
	//炸弹
	if (square.state == 7) {
		//左右边缘检测
		if (square.tmp[0][0] >= mapWidth || square.tmp[0][0] < 0) {
			square.sync();
			return;
		}
		//获取炸弹所在列最低点空点
		for (i = mapHeight - 1; i >= square.square[0][1]; i--) {
			if (myMap.map[i][square.square[0][0]] == 0) {
				break;
			}
		}
		//判断炸弹是否在该空点上
		if (i == square.square[0][1] || square.square[0][1] == mapHeight - 1) {
			myMap.remove(i);
			square.birth();
			return;
		}
		square.arrive();
		return;
	}
	//普通方块
	for (i = 0; i < 4; i++) {
		//左右边缘检测
		if (square.tmp[i][0] >= mapWidth || square.tmp[i][0] < 0) {
			square.sync();
			return;
		}
		//检测下一步移动是否可执行
		if (myMap.map[square.tmp[i][1]][square.tmp[i][0]] == 1) {
			if (flag == false) {
				square.sync();
				return;
			}
			//叠加、出生
			myMap.join(square);
			square.birth();
			return;
		}
	}
	//执行移动实现操作
	square.arrive();
}
//背景音乐
void BkMusic() {
	if (clock() / 1000 >= Time + 350) {
		mciSendString("close ./audio/You.mp3", NULL, 0, 0);
		mciSendString("open ./audio/You.mp3", NULL, 0, 0);
		mciSendString("play ./audio/You.mp3", NULL, 0, 0);
		Time = clock() / 1000;
	}
}
//初始化背景和文字的颜色、样式
void InitSet() {
	setbkcolor(RGB(199, 199, 199));
	setfillcolor(RGB(0, 255, 0));
	setcolor(RGB(255, 0, 255));
	settextstyle(25, 13, "楷体");
	setbkmode(TRANSPARENT);
	cleardevice();
	IMAGE img;
	loadimage(&img, "./image/TETRIS.jpg", screenWidth + 6 * picWidth, screenHeight);
	putimage(0, 0, &img);
}
//单局游戏
bool PlayGame() {
	bool flag;
	int T = 0, times = 0;
	Square square;
	Map myMap;
	while (true) {
		Sleep(times);
		BkMusic();
		//元素打印
		BeginBatchDraw();
		cleardevice();
		myMap.show();
		square.show();
		myMap.score.show();
		EndBatchDraw();
		//按键检测
		flag = Key_Down(square, myMap, T, times);
		//碰撞检测
		Carsh(myMap, square, flag);
		//重来？
		ReGame(myMap, square);
		if (regame) {
			regame = false;
			return true;
		}
	}
}
//程序入口
int main() {
	time_t t;
	srand((unsigned int)time(&t));
	initgraph(screenWidth + 6 * picWidth, screenHeight);
	InitSet();
	BkMusic();
	Sleep(2500);
	while (PlayGame());
	closegraph();
	return 0;
}