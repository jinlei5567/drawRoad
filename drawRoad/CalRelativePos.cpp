
#include <iostream>

#include "afLocalCartesian.h"


void plotLine(std::map<uint64_t, line> lanecenter, std::map<int, line> laneLine, std::shared_ptr<LocalCartesian> LocalCartesianPtr, double head, linVec& lineVecX, linVec& lineVecy, linVec& bounVecX, linVec& bounVecY, linVec& centerVecX, linVec& centerVecY, int linkid)
{

	std::vector<double> bounX, bounY;
	for (auto lineiter = laneLine.begin(); lineiter != laneLine.end(); lineiter++)
	{

		std::vector<point> linePoint = lineiter->second.points;
		std::vector<double> laneX, laneY;

		if (lineiter->second.side != 2)
		{
			for (int i = 0; i < linePoint.size() -1 ; i = i + 1)
			{
				double a, b, c;
				LocalCartesianPtr->LLAToEgo(linePoint[i].lon, linePoint[i].lat, 0, head, a, b, c);
#ifdef PART_SHOW
				if (a > -10 && a < 50 && b > -20 && b < 20)
#else
				//if (a > -200 && a < 200 && b > -50 && b < 50)
				//if(lineiter->second.linkId == linkid)
#endif
				{
					laneX.push_back(a);
					laneY.push_back(b);
				}
			}
			lineVecX.push_back(laneX);
			lineVecy.push_back(laneY);
		}
		else
		{
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
		}
	}

}


int main()
{

	std::string file = "H:\\jl\\RoadFusion_log\\1017_1753Road.log";

	readLog mSplitData;
	mSplitData.setFileName(file);
	mSplitData.parseLog();
	std::vector<point> GNSS = mSplitData.GNSSPos;

	double startlat = 41.6994942;
	double startlon = 123.4413639;
	double head = 45.5538597;

	auto offest = 1691213490728; //1685024021861;// ;

	uint64_t time = 0;// 1691213363177;// 1691293304110; // 1691293202654; //0731   1688372933537 -  1688372996639
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
		



	}
	std::cout << "Hello World!\n";
}


