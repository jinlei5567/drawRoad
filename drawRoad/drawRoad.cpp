// drawRoad.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define CRTDBG_MAP_ALLOC

#include <iostream>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "matplotlibcpp.h"
#include "afLocalCartesian.h"
#include "enu.h"
#include "readData.h"
#include "lineFit.h"
 

#define cut 0

#define PART_SHOW				(1)
//#define AUTO_SHOW			 	(1)
#define DRAW_LD_LINE			(1)
//#define DRAW_HDMAP		    (1)
//#define SAVE_MAP_FILE_BIN	    (1)
//#define DRAW_HEAD				(1)
#define DRAW_BOUND				(1)
#define DRAW_AG

struct MapPoint2D
{
	double lon;
	double lat;
};

struct MapLineSave
{
	uint32_t    linkid;
	uint32_t    laneid;
	uint32_t    lineid;
	uint32_t    side;
	uint32_t    sequence;
	uint32_t    pointSize;
	int         lanemarkingtype;
	uint64_t    mapTime;
	MapPoint2D  point[1700];
};


void SaveMapFile(std::map<int, line> laneLine)
{


	MapLineSave lineSave;
	std::ofstream fileOpen("map.bin", std::ios::binary | std::ios::app);
	if (!fileOpen)
	{
		std::cout << "make failed" << std::endl;
		return;
	}

	int saveBytes = 0;
	for (auto lineIter = laneLine.begin(); lineIter != laneLine.end(); lineIter++)
	{
		
		{
			lineSave.lineid = lineIter->second.lineid;
			lineSave.linkid = lineIter->second.linkId;
			lineSave.laneid = lineIter->second.lineid;
			lineSave.side = 1;
			lineSave.sequence = 1;
			lineSave.lanemarkingtype = 12;
			lineSave.mapTime = 123345;
			lineSave.pointSize = lineIter->second.points.size() + 1;

			std::vector<point> linePoint = lineIter->second.points;
			int saveLength = sizeof(int) * 7 + sizeof(uint64_t) + lineSave.pointSize * sizeof(MapPoint2D);
			for (int i = 0; i < linePoint.size(); i++)
			{
				lineSave.point[i].lon = linePoint[i].lon;
				lineSave.point[i].lat = linePoint[i].lat;
			}

			lineSave.point[lineSave.pointSize -1].lon = 0.0;
			lineSave.point[lineSave.pointSize -1 ].lat = 0.0;

			fileOpen.write((char*)&lineSave, saveLength); // fwrite以char *的方式进行写出，做一个转化
			saveBytes += saveLength;
		}

	}
	fileOpen.close();

	//gettimeofday(&time2, NULL);
	//uint64_t rec_time = (time2.tv_sec - time1.tv_sec) * 1000000 + (time2.tv_usec - time1.tv_usec); // us
	//LOGD("[0925Test] Save Map Line %d OK, total %d bytes, cost %ld us\n", mpLocalHDMap[readIndex]->lanemark_map.size(), saveBytes, rec_time);

}




namespace plt = matplotlibcpp;

typedef std::vector<std::vector<double>> linVec;

void plotLine(std::map<uint64_t, line> lanecenter, std::map<int, line> laneLine, std::shared_ptr<LocalCartesian> LocalCartesianPtr, double head, linVec& lineVecX, linVec& lineVecy, linVec& bounVecX, linVec& bounVecY, linVec& centerVecX, linVec& centerVecY, int linkid)
{

	std::vector<double> bounX, bounY;
	for (auto lineiter = laneLine.begin(); lineiter != laneLine.end(); lineiter++)
	{

		std::vector<point> linePoint = lineiter->second.points;
		std::vector<double> laneX, laneY;

		if (lineiter->second.side != 2)
		{
			for (int i = 0; i < linePoint.size() ; i = i + 1)
			{
				double a, b, c;
				LocalCartesianPtr->LLAToEgo(linePoint[i].lon, linePoint[i].lat, 0, head, a, b, c);
#ifdef PART_SHOW
				if (a > -200 && a < 200 && b > -50 && b < 50)
#else
				//if (a > -200 && a < 200 && b > -50 && b < 50)
				//if(lineiter->second.linkId == linkid)
#endif
				{
					laneX.push_back(a);
					laneY.push_back(b);
					//cout << "lineid : " << lineiter->second.lineid << endl;
				}
			}
			lineVecX.push_back(laneX);
			lineVecy.push_back(laneY);
		}
		else
		{
			/*
			for (int i = 0; i < linePoint.size(); i = i + 1)
			{
				double a, b, c;
				LocalCartesianPtr->LLAToEgo(linePoint[i].lon, linePoint[i].lat, 0, head, a, b, c);
#ifdef PART_SHOW
				if (a > -10 && a < 50 && b > -20 && b < 20)
#else
				//if (a > -100 && a < 200 && b > -50 && b < 50)
#endif
					//if (lineiter->second.linkId == linkid)
				{
					laneX.push_back(a);
					laneY.push_back(b);
				}

			}

			bounVecX.push_back(laneX);
			bounVecY.push_back(laneY);
			*/
		}
	}

}


int main()
{

	//std::string file = "E:\\ShareMem\\RF_Part_Log\\1207_1031Road.log";
	//std::string file = "H:\\jl\\RoadFusion_log\\ehp回灌\\回灌log\\1004_1714Road_taijia.log";
	//std::string file = "H:\\jl\\RoadFusion_log\\Transfered_Log\\1214_1723Road.log"; 
	std::string file = "H:\\jl\\RoadFusion_log\\0110_1606Road.log";
	//std::string file = "H:\\jl\\RoadFusion_log\\405Chongqing\\20231219181403_武汉_重庆_晴天_白天_城市道路_426_486_匝道内压左线冲出车道_Road.log";
	//std::string file = "H:\\jl\\ParseSut\\20231217144542_武汉_武汉_晴天_白天_城市道路_452_512_向左变道后自车骑左线冲出车道_Road.log";
	//std::string file = "H:\\jl\\RoadFusion_log\\Recently\\1123_1642Road.log"; 
	//std::string file = "H:\\jl\\RoadFusion_log\\TestDepart\\2ndRing_Wuhan\\0808_1507_erhuan_Road.log";

	readLog mSplitData;
	mSplitData.setFileName(file);
	mSplitData.parseLog();
	std::vector<point> GNSS = mSplitData.GNSSPos;
#ifdef BYD_PROJ
	std::map<int, line> laneLine = mSplitData.laneLine;
	std::map<uint64_t, line> laneCenter = mSplitData.laneCenter;
	std::vector<std::pair<uint64_t, line>> centerLine = mSplitData.centerLine;
#endif

	std::map<uint64_t, roadLines> OutLine = mSplitData.OutLine; // 道路融合输出的车道线，不包含视觉车道线
	std::map<uint64_t, roadLines>  Outbound = mSplitData.Outbound;  // 道路融合输出的道路边缘，不包含视觉部分

#ifdef DRAW_HDMAP
	//std::string fileMap = "H:\\jl\\RoadFusion_log\\hdmapLineBin\\WuhanToShanghai_HDMapLine.txt"; 
	std::string fileMap = "H:\\jl\\RoadFusion_log\\hdmapLineBin\\WuHan_HDMapLine.txt";
	//std::string fileMap = "H:\\jl\\RoadFusion_log\\hdmapLineBin\\WuhanToChongqing_HDMapLine.txt";
	readLog mSplitDataMap;
	mSplitDataMap.setFileName(fileMap);
	mSplitDataMap.parseLog();
	std::map<int, line> laneLine = mSplitDataMap.laneLine;
	std::map<uint64_t, line> laneCenter = mSplitDataMap.laneCenter;
	std::vector<std::pair<uint64_t, line>> centerLine = mSplitDataMap.centerLine;
#endif

	ENU m_ENU;
	double startlat = 41.6994942;
	double startlon = 123.4413639;
	double head = 45.5538597;

	auto offest = 1691213490728; //1685024021861;// ;

	uint64_t time = 1557028;// 1691213363177;// 1691293304110; // 1691293202654; //0731   1688372933537 -  1688372996639
	uint64_t timEnd = 1688614410000; //1691666590301 1675946010122;1675945764050,//1675945764291 //1675945764652 //1675945764652


	plt::figure_size(1280, 960);

	//plt::set_aspect(1);

	std::map<uint64_t, point >  GNSSPosMap = mSplitData.GNSSPosMap;


	//auto failTime = mSplitData.failTime;

	std::vector<std::vector<double>> crreVecX, crreVecY, lineVecTestX, lineVecTestY, centerX, centerY, LDBoudaryX, LDBoudaryY;

	uint64_t lastTime = 0;
	std::shared_ptr<LocalCartesian> LocalCartesianPtr = std::make_shared<LocalCartesian>();

	std::cout << std::endl << GNSS.size() << std::endl;
	int linkId = 0;
	for (int a = 0; a < GNSS.size(); a = a + 1)
	{
		plt::grid(1);
		plt::set_aspect(1);
		if (GNSS[a].timeStamp >= time && GNSS[a].timeStamp != lastTime)
		{
			std::cout << std::endl << GNSS[a].timeStamp << std::endl;

			std::cout << "dt " << GNSS[a].timeStamp - lastTime << std::endl;
			lastTime = GNSS[a].timeStamp;

			startlat = GNSS[a].lat;
			startlon = GNSS[a].lon;
			head = GNSS[a].heading;
			linkId = GNSS[a].linkId;

			if (a > 2)
			{
				head = head + GNSS[a - 1].heading;
				head = head / 2.0;
			}


			printf("%0.7f,%0.7f,%0.7f \n", startlat, startlon, head);

			LocalCartesianPtr->Reset(startlon, startlat, 0);
			double startx, starty, startz;
			LocalCartesianPtr->LLAToEgo(startlon, startlat, 0, head, startx, starty, startz);
			std::vector<double> pointX, pointY, times, heads, pointXEKF, pointYEKF, timesEKF, headEKF;
#ifdef DRAW_AG
			auto GNSSPosOrg = mSplitData.GNSSPosOrg;
			for (int i = 0; i < GNSSPosOrg.size(); i++)
			{
				double a, b, c;
				LocalCartesianPtr->LLAToEgo(GNSSPosOrg[i].lat, GNSSPosOrg[i].lon, 0, head, a, b, c);
				if (a > -20 && a < 50 && b > -50 && b < 50)
				{
					pointXEKF.push_back(a);
					pointYEKF.push_back(b);
				}

			}
#endif
			for (auto iter = GNSSPosMap.begin(); iter != GNSSPosMap.end(); iter++)
			{
				double a, b, c;
				LocalCartesianPtr->LLAToEgo(iter->second.lon, iter->second.lat, 0, head, a, b, c);
#ifdef PART_SHOW
				if (a > -10 && a < 50 && b > -20 && b < 20)
#else
				//if (a > -50 && a < 200 && b > -50 && b < 50)
#endif
					//if(iter->second.timeStamp > time  && timEnd > iter->second.timeStamp)
				{
					pointX.push_back(a);
					pointY.push_back(b);
					times.push_back(iter->second.timeStamp);
					heads.push_back(iter->second.heading);
				}
			}

			time = GNSS[a].timeStamp;

			linVec lineVecX, lineVecy, bounVecX, bounVecY, centerLineVecX, centerLineVecY;

#ifdef DRAW_HDMAP
			plotLine(laneCenter, laneLine, LocalCartesianPtr, head, lineVecX, lineVecy, bounVecX, bounVecY, centerLineVecX, centerLineVecY, linkId);

			// 车道线
			for (int i = 0; i < lineVecX.size(); i++)
			{
				plt::plot(lineVecX[i], lineVecy[i], "r-*");
			}
			// 边界线	linVec lineVecX, lineVecy, bounVecX, bounVecY;
			for (int i = 0; i < bounVecX.size(); i++)
			{
				plt::plot(bounVecX[i], bounVecY[i], "g-");
			}

#endif
		
			////auto roadLines = mSplitData.roadLine;

#ifdef SAVE_MAP_FILE_BIN
			SaveMapFile(laneLine);
#endif

			crreVecX.clear();
			crreVecY.clear();
			auto iter = OutLine.find(time);
			if (iter != OutLine.end())
			{

				std::cout << iter->second.count << std::endl;
				for (int i = 0; i < iter->second.count; i++)
				{
					ARoadLine &Laneline = iter->second.line[i];

					//if (Laneline.Id != 66430236) continue;

					double A0 = Laneline.LineA0;
					double A1 = Laneline.LineA1;
					double A2 = Laneline.LineA2;
					double A3 = Laneline.LineA3;
					std::vector<double> testX, testY, alllaneX, alllaneY;


					for (int a = Laneline.LineStart; a < Laneline.LineEnd; a = a + 1000)
					{
						double x = a / 1000.0;
						//if (x > -10 && x < 50)
						{

							double y = A3 * x * x * x + A2 * x * x + A1 * x + A0;
							testX.push_back(x);
							testY.push_back(y);
						}

					}

					double x = Laneline.LineEnd / 1000.0;
					double y = A3 * x * x * x + A2 * x * x + A1 * x + A0;
					testX.push_back(x);
					testY.push_back(y);
					crreVecX.push_back(testX);
					crreVecY.push_back(testY);
				}
				OutLine.erase(iter);

			}


			LDBoudaryX.clear();
			LDBoudaryY.clear();
			auto bounditer = Outbound.find(time);
			if (bounditer != Outbound.end())
			{

				std::cout << bounditer->second.count << std::endl;
				for (int i = 0; i < bounditer->second.count; i++)
				{
					ARoadLine& Laneline = bounditer->second.line[i];

					double A0 = Laneline.LineA0;
					double A1 = Laneline.LineA1;
					double A2 = Laneline.LineA2;
					double A3 = Laneline.LineA3;
					std::vector<double> testX, testY, alllaneX, alllaneY;


					for (int a = Laneline.LineStart; a < Laneline.LineEnd; a = a + 1000)
					{
						double x = a / 1000.0;
						//if (x > -10 && x < 50)
						{

							double y = A3 * x * x * x + A2 * x * x + A1 * x + A0;
							testX.push_back(x);
							testY.push_back(y);
						}

					}

					double x = Laneline.LineEnd / 1000.0;
					double y = A3 * x * x * x + A2 * x * x + A1 * x + A0;
					testX.push_back(x);
					testY.push_back(y);
					LDBoudaryX.push_back(testX);
					LDBoudaryY.push_back(testY);
				}
				Outbound.erase(bounditer);
			}

#ifdef DRAW_LD_LINE
			auto LDLines = mSplitData.LDLine;
			lineVecTestX.clear();
			lineVecTestY.clear();
			for (int i = 0; i < LDLines.size(); i++)
			{
				if (LDLines[i].first == time)
				{
					ARoadLine Laneline = LDLines[i].second;

					float A0 = Laneline.LineA0;
					float A1 = Laneline.LineA1;
					float A2 = Laneline.LineA2;
					float A3 = Laneline.LineA3;
					//std::cout << Laneline.Id << std::endl;
					std::vector<double> testX, testY;

					std::vector<double> laneX, laneY, vecLat, vecLon;

					for (int a = Laneline.LineStart / 1000; a < Laneline.LineEnd / 1000; a = a + 1)
					{
						double x = a;
						double y = A3 * x * x * x + A2 * x * x + A1 * x + A0;
						testX.push_back(x);
						testY.push_back(y);
					}

					lineVecTestX.push_back(testX);
					lineVecTestY.push_back(testY);
				}
			}
#endif
			printf("[1016] X size %ld, Y size %ld \n", lineVecTestX.size(), lineVecTestY.size());
			////当前定位的视觉车道线
			for (int i = 0; i < lineVecTestX.size(); i++)
			{
				plt::plot(lineVecTestX[i], lineVecTestY[i], "g*-");
			}


			//当前定位的地图边界线
#ifdef DRAW_BOUND
			for (int i = 0; i < LDBoudaryX.size(); i++)
			{
				plt::plot(LDBoudaryX[i], LDBoudaryY[i], "c-");
			}
#endif
			////当前定位地图车道线
			for (int i = 0; i < crreVecX.size(); i++)
			{
				plt::plot(crreVecX[i], crreVecY[i], "b-");
			}

#ifdef DRAW_AG
			plt::plot(pointXEKF, pointYEKF, "b-*");
#endif
			//plt::plot(pointX, pointY, "y-*");
			pointX.clear();
			pointY.clear();
			pointX.push_back(startx);
			pointY.push_back(starty);
			plt::named_plot(std::to_string(GNSS[a].timeStamp), pointX, pointY, "g*");
			//plt::plot(pointX, pointY, "g*");
			plt::legend();
			//plt::text(800, 600, std::to_string(GNSS[i].timeStamp));
			//plt::tight_layout();

#ifdef DRAW_HEAD
			plt::figure_size(1280, 960);
			plt::grid(1);
			plt::plot(times, heads, "g*-");
#endif

#ifdef AUTO_SHOW
			plt::show(false);
#else
			plt::show();
#endif
			plt::pause(0.5);
			plt::clf();
			//Sleep(10);
			//plt::close();

		}

	}
	std::cout << "Hello World!\n";
}


