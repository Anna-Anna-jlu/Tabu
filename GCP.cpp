#include<time.h>
#include<fstream>
#include<stdlib.h>
#include<stack>
#include<vector>
#include<climits>
#include<iostream>
#include<sstream>
using namespace std;


/*
1.采用类似邻接链表存储图，使用二维数组记录每个顶点的边，另开一数组记录对应边的条数
2.在循环外计算二维数组的一位地址，进行优化，避免重复计算
3.计算冲突差量相同时，计数相同个数，每次根据概率选取，第一次为概率1，第二次概率为1/2，第三次概率为1/3...
*/

int N;//图的顶点数目
int** g;//邻接图
int* v_edge;//每个顶点连接的边数
int* sol;//结点对应颜色
int f;//冲突值
int** TabuTenure;//禁忌表
int** Adjacent_Color_Table;//邻接颜色表
int K;//颜色数量
int delt;//移动增量
int Best_f;//历史最好的冲突值
int node;//每次移动的结点
int color;//每次移动的颜色
int iter;//迭代次数

//读取文件数据，创建图结构
void read_file() {
	fstream f("D:\\Tabu\\data\\data\\DSJC500.5.col");
	vector<string> words;
	string line;
	bool flag = false;
	int v1, v2,tmp;
	while (getline(f, line))
	{
		if (flag)
		{
			//用于存放分割后的字符串 
			vector<string> res;
			res.clear();
			//暂存从word中读取的字符串 
			string result;
			//将字符串读到input中 
			stringstream Input(line);
			//依次输出到result中，并存入res中 
			while (Input >> result)
				res.push_back(result);
			v1 = stoi(res[1]) - 1;
			v2 = stoi(res[2]) - 1;
			tmp = ++v_edge[v1];
			g[v1][tmp - 1] = v2;
			tmp = ++v_edge[v2];
			g[v2][tmp - 1] = v1;

		}
		else if (line[0] == 'p')
		{
			flag = true;
			//用于存放分割后的字符串 
			vector<string> res;
			res.clear();
			//暂存从word中读取的字符串 
			string result;
			//将字符串读到input中 
			stringstream Input(line);
			//依次输出到result中，并存入res中 
			while (Input >> result)
				res.push_back(result);
			N = stoi(res[2]);
			cout << "顶点数： " << N << endl;
	
				g = new int* [N];//初始化图
				v_edge = new int[N];
				for (int i = 0; i < N; i++) {
					g[i] = new int[N];
					v_edge[i] = 0;
				}
				for (int i = 0; i < N; i++)
					for (int j = 0; j < N; j++)
						g[i][j] = 0;

		}
		
	}

	f.close();

}

//初始化，分组顶点颜色，计算初始冲突值，初始化邻接颜色表
void initialization(int num) {
	K = num;
	f = 0;
	
	sol = new int[N];
	Adjacent_Color_Table = new int* [N];
	TabuTenure = new int* [N];

	for (int i = 0; i < N; i++) {
		Adjacent_Color_Table[i] = new int[K];
		TabuTenure[i] = new int[K];
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < K; j++) {
			Adjacent_Color_Table[i][j] = 0;
			TabuTenure[i][j] = 0;
		}
	}

	for (int i = 0; i < N; i++)
		sol[i] = rand() % K;
	int num_edge;
	int* h_graph;
	int adj_color;
	int c_color;
	for (int i = 0; i < N; i++) {
		num_edge = v_edge[i];
		h_graph = g[i];//首先计算出数组首地址，避免循环中重复计算，优化
		c_color = sol[i];
		for (int u = 0; u < num_edge; u++) {//初始化冲突值和邻接颜色表
			adj_color = sol[h_graph[u]];
			if (c_color == adj_color) f++;
			Adjacent_Color_Table[i][adj_color]++;
		}
	}
	f = f / 2;//冲突值重复计算
	Best_f = f;
	std::cout << "初始冲突数： " << f << endl;
}


void findmove() {
	int tem_node1;//非禁忌移动临时节点
	int tem_color1;//非禁忌移动临时颜色
	int tem_node2;//禁忌移动临时节点
	int tem_color2;//禁忌移动临时颜色
	delt = 99999;
	int tabu_delt = 99999;//禁忌移动临时增量
	int tmp;
	int count = 1, tabu_count = 1;//相同增量值计数
	int c_color;//当前结点颜色
	int* h_color;//邻接颜色表行首指针
	int* h_tabu;//禁忌表行首指针
	int c_color_table;//当前结点颜色表的值
	int A = Best_f - f;//冲突值差量
	for (int i = 0; i < N; i++) {
		c_color = sol[i];
		h_color = Adjacent_Color_Table[i];
		c_color_table = h_color[c_color];
		if (h_color[c_color] > 0) {//对应邻接颜色表值大于零
			h_tabu = TabuTenure[i];
			for (int j = 0; j < K; j++) {
				if (c_color != j) {
					tmp = h_color[j] - c_color_table;//要移动位置减当前颜色值
					if (h_tabu[j] <= iter) {//非禁忌移动
						if (tmp <= delt) {
							if (tmp < delt) {//小于直接赋值
								count = 1;
								delt = tmp;
								tem_node1 = i;//记录对应的节点与颜色
								tem_color1 = j;
							}
							else
							{
								int r = rand() % count + 1;//优化部分，相同时，100%取第一个值，50%取第二个值....
								if (r == 1)
								{
									delt = tmp;
									tem_node1 = i;//记录对应的节点与颜色
									tem_color1 = j;
								}	
								count++;
							}

						}
					}
					else {//禁忌移动
						if (tmp <= tabu_delt) {
							if (tmp < tabu_delt) {
								tabu_delt = tmp;
								tabu_count = 1;
								tem_node2 = i;
								tem_color2 = j;
							}
							else
							{
								int r = rand() % tabu_count + 1;
								if (r == 1)
								{
									tem_node2 = i;
									tem_color2 = j;
									tabu_delt = tmp;
								}
								tabu_count++;
									
							}
						}
					}
				}
			}
		}
	}
	if (tabu_delt < A && tabu_delt < delt) {//禁忌
		delt = tabu_delt;
		node = tem_node2;
		color = tem_color2;
	}
	else {
		node = tem_node1;
		color = tem_color1;
	}
}
//更新值
void makemove() {
	f = delt + f;
	if (f < Best_f) Best_f = f;
	int old_color = sol[node];
	sol[node] = color;
	TabuTenure[node][old_color] = iter + f + rand() % 10;
	int* h_graph = g[node];
	int num_edge = v_edge[node];
	int tmp;
	for (int i = 0; i < num_edge; i++) {//更新邻接颜色表
		tmp = h_graph[i];
		Adjacent_Color_Table[tmp][old_color]--;
		Adjacent_Color_Table[tmp][color]++;
	}
}

//禁忌搜索
void tabusearch() {
	read_file();
	int numofcolor;
	double start, end;
	double elapsed_time;
	while (cin >> numofcolor)
	{
		srand(clock());
		initialization(numofcolor);
		start = clock();
		iter = 0;
		while (f > 0) {
			iter++;
			findmove();
			makemove();
		}
		end = clock();
		elapsed_time = (double(end - start)) / CLOCKS_PER_SEC;
		std::cout << "迭代次数: " << iter << "  迭代时间:  " << elapsed_time << "(s)"  << endl;
	}
}


//释放内存
void delete_alloc() {
	for (int i = 0; i < N; i++) {
		delete[]TabuTenure[i];
		delete[]Adjacent_Color_Table[i];
		delete[]g[i];
	}
	delete[]sol;
	delete[]TabuTenure;
	delete[]Adjacent_Color_Table;
	delete[]g;
}

int main() {
	tabusearch();
	delete_alloc();
	system("pause");
	return 0;
}
