﻿
#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <fstream>
using namespace std;
using namespace Eigen;

int main(int argc, char **argv) {

  double ar = 1.0, br = 2.0, cr = 1.0;         // 真实参数值
  double ae = 2.0, be = -1.0, ce = 5.0;        // 估计参数值
  int N = 100;                                 // 数据点
  double w_sigma = 2.0;                        // 噪声Sigma值
  double inv_sigma = 1.0 / w_sigma;
  cv::RNG rng;                                 // OpenCV随机数产生器


  std::string filename = "data.csv";
  std::ofstream outputFile(filename);
  // 检查文件是否成功打开
  if (!outputFile.is_open()) {
      std::cerr << "无法打开文件 " << filename << std::endl;
      return 1;
  }

  vector<double> x_data, y_data;      // 数据
  for (int i = 0; i < N; i++) {
    double x = i / 100.0;
    x_data.push_back(x);
    y_data.push_back(exp(ar * x * x + br * x + cr) + rng.gaussian(w_sigma * w_sigma));

    //outputFile << x <<","  << y_data.at(i);
    //outputFile << "\n"; // 写入新行


  }



  std::vector<double> A(10, 0);

  chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

  for (int all_nums = 0; all_nums < 10000; all_nums++)
  {
      for (int k = 0; k < 10; k++)
      {
          double grad = 0;
          for (int i = 0; i < N; i++)
          {
              double yi = 0;

              for (int j = 0; j < 10; j++)
              {
                  yi += A[j] * std::pow(x_data.at(i), j);


              }

              grad += -2*(y_data.at(i) - yi )* std::pow(x_data.at(i), k);

          }
          A[k] += -grad * 0.01;




      }

      if (all_nums % 100 == 0)
      {
          double error = 0;
          for (int i = 0; i < N; i++)
          {
              double yi = 0;


              for (int j = 0; j < 10; j++)
              {
                  yi += A[j] * std::pow(x_data.at(i), j);


              }

              error += (y_data.at(i) - yi) * (y_data.at(i) - yi);



          }
          cout << "error =  " << error << endl;
      }





  }



  vector<double> x1_data, y1_data;      // 数据
  for (int i = 0; i < N; i++) {
      double x = i / 100.0;
      x1_data.push_back(x);

      double yi = 0;
      for (int j = 0; j < 10; j++)
      {
          yi += A[j] * std::pow(x, j);


      }

      y1_data.push_back(yi);

      //outputFile << x <<","  << y_data.at(i);
      //outputFile << "\n"; // 写入新行


  }


  vector<double> x2_data, y2_data;      // 数据


  chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
  chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
  cout << "solve time cost = " << time_used.count() << " seconds. " << endl;


  // 开始Gauss-Newton迭代
  int iterations = 100;    // 迭代次数
  double cost = 0, lastCost = 0;  // 本次迭代的cost和上一次迭代的cost

  chrono::steady_clock::time_point t11 = chrono::steady_clock::now();
  for (int iter = 0; iter < iterations; iter++)
  {

    Matrix3d H = Matrix3d::Zero();             // Hessian = J^T W^{-1} J in Gauss-Newton
    Vector3d b = Vector3d::Zero();             // bias
    cost = 0;

    for (int i = 0; i < N; i++) {
      double xi = x_data[i], yi = y_data[i];  // 第i个数据点
      double error = yi - exp(ae * xi * xi + be * xi + ce);
      Vector3d J; // 雅可比矩阵
      J[0] = -xi * xi * exp(ae * xi * xi + be * xi + ce);  // de/da
      J[1] = -xi * exp(ae * xi * xi + be * xi + ce);  // de/db
      J[2] = -exp(ae * xi * xi + be * xi + ce);  // de/dc

      H += inv_sigma * inv_sigma * J * J.transpose();
      b += -inv_sigma * inv_sigma * error * J;

      cost += error * error;
    }

    // 求解线性方程 Hx=b
    Vector3d dx = H.ldlt().solve(b);
    if (isnan(dx[0])) {
      cout << "result is nan!" << endl;
      break;
    }

    if (iter > 0 && cost >= lastCost) {
      cout << "cost: " << cost << ">= last cost: " << lastCost << ", break." << endl;
      break;
    }

    ae += dx[0];
    be += dx[1];
    ce += dx[2];

    lastCost = cost;

    cout << "total cost: " << cost << ", \t\tupdate: " << dx.transpose() <<
         "\t\testimated params: " << ae << "," << be << "," << ce << endl;





    
  }
    for (int i = 0; i < N; i++) 
    {
        double x = i / 100.0;
        x2_data.push_back(x);

        double yi = exp(ae * x * x + be * x + ce);  // de/db
        y2_data.push_back(yi);

        //outputFile << x <<","  << y_data.at(i);
        //outputFile << "\n"; // 写入新行
    }

  for (int i = 0; i < N; i++) 
  {


  outputFile << x_data.at(i) <<","  << y_data.at(i) << "," << x1_data.at(i) << "," << y1_data.at(i) << "," << x2_data.at(i) << "," << y2_data.at(i);
      outputFile << "\n"; // 写入新行
  }

  outputFile.close();


  chrono::steady_clock::time_point t22 = chrono::steady_clock::now();
  chrono::duration<double> time_used2 = chrono::duration_cast<chrono::duration<double>>(t22 - t11);
  cout << "solve time cost = " << time_used2.count() << " seconds. " << endl;

  cout << "estimated abc = " << ae << ", " << be << ", " << ce << endl;
  return 0;
}
