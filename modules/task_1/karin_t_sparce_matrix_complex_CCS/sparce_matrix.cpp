// Copyright 2020 Karin Timofey
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <cmath>
#include <iostream>
#include <complex>
#include "../../../modules/task_1/karin_t_sparce_matrix_complex_CCS/sparce_matrix.h"

std::vector<int> getRandomVector(int  sz) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::vector<int> vec(sz);
    for (int  i = 0; i < sz; i++) { vec[i] = gen() % 100; }
    return vec;
}

SparceMatrix::SparceMatrix(int _nCol, int _nRow, int not_null_count) {
  if (_nCol <= 0 || _nRow <= 0)
    throw "negative size";
  if (_nCol * _nRow < not_null_count)
    throw "not null count does'n match the size";
  nCol = _nCol;
  nRow = _nRow;
  point.resize(nCol);
  std::mt19937 ran;
  ran.seed(static_cast<unsigned int>(time(0)));
  unsigned count = not_null_count;
  for (int i=0; i<nCol; i++) {
    int next = std::min(count, ran()%nRow);
    count -= next;
    point[i]=next;
  }
  while (count != 0)
  {
    for (int i = 0; i < nCol; i++){
      if(point[i] < nRow) {
        point[i]++;
        count--;
      }
      if (count == 0)
        break;
    }
  }
  std::random_shuffle(point.begin(), point.end());
  for (int i=0; i < nCol-1; i++) {
     point[i+1] += point[i];
  }

  for (int i = 0; i < nCol; i++)
  {
    int k;
    if (i==0)
      k = point[i];
    else if (i == nCol - 1)
      k = not_null_count - point[i - 1];
    else
      k = point[i]-point[i-1];
    std::vector<int> col;
    for (int j=0; j<k; j++) {
      int r = ran()%nRow;
      bool added = false;
      while (added == false) {
        bool exist = false;
        for (int i=0; i<col.size(); i++) {
          if (col[i]==r) {
            exist = true;
            break;
          }
        }
        if (exist == false) {
          col.push_back(r);
          added = true;
        }
        else
          r = (r+1)%nRow;
      }
    }
    std::sort(col.begin(), col.end());
    for (int i=0; i<k; i++)
      row_number.push_back(col[i]);
  }

  for (int i = 0; i < not_null_count; i++) {
    std::complex<int> compl(ran()%100, 0);
    val.push_back(compl);
  }
}

SparceMatrix::SparceMatrix(std::vector<std::vector<std::complex<int>>> matr) {
  /*for (int i = 0; i < matr.size(); i++) {
    for (int j=0; j < matr.size(); i++)
  }*/
}

SparceMatrix::SparceMatrix(int _nCol, int _nRow, std::vector<std::complex<int>> _val,
                            std::vector<int> _row_number, std::vector<int> _point) {
  val = _val;
  row_number=_row_number;
  point = _point;
  nCol = _nCol;
  nRow = _nRow;
}

SparceMatrix SparceMatrix::Transpose() {
  SparceMatrix Tr(this->nRow, this->nCol);
  std::vector<std::vector<int>> matr(nRow);
  std::vector<int> cols;
  int iter=0;
  for (int i=0; i<row_number.size(); i++) {
    while((i >= point[iter]) && (iter!=nCol-1))
      iter++;
    matr[row_number[i]].push_back(i);
    cols.push_back(iter);
  }
  Tr.point[0]=matr[0].size();
  for (int i=1; i<nRow; i++)
    Tr.point[i]= Tr.point[i-1]+matr[i].size();
  for (int i=0; i<nRow; i++)
    for (int j = 0; j < matr[i].size(); j++) {
      Tr.val.push_back(val[matr[i][j]]);
      Tr.row_number.push_back(cols[matr[i][j]]);
    }
  return Tr;
}

SparceMatrix SparceMatrix::operator*(const SparceMatrix& B) {
  SparceMatrix ATr=this->Transpose();
  SparceMatrix Res(B.nCol, this->nRow);
  for (int i = 0; i < B.nCol; i++) {
    for (int j = 0; j < ATr.nCol; j++) {
      int k1, k2;
      if (j==0)
        k1=0;
      else
        k1 = ATr.point[j-1];
      if (i == 0)
        k2 = 0;
      else
        k2 = B.point[i - 1];
      std::complex<int> sum = (0, 0);
        int stop1, stop2;
        if (j==ATr.nCol-1)
          stop1 = ATr.val.size();
        else
          stop1 = ATr.point[j];
        if (i == B.nCol - 1)
          stop2 = B.val.size();
        else
          stop2 = B.point[i];
      while(k1<stop1 && k2<stop2) {
        if (ATr.row_number[k1] == B.row_number[k2]) {
          sum+=ATr.val[k1] * B.val[k2];
          k1++;
          k2++;
        }
        else if (ATr.row_number[k1] > B.row_number[k2])
          k2++;
        else
          k1++;
      }
      if (sum != (0, 0)) {
        Res.val.push_back(sum);
        Res.row_number.push_back(j);
      }
    }
    Res.point[i]=Res.val.size();
  }
  return Res;
}

void SparceMatrix::Print() {
  std::cout<<"val: ";
  for (int i=0; i<val.size(); i++)
    std::cout << val[i] << ", ";
  std::cout << std::endl<<std::endl << "row_number: ";
  for (int i = 0; i < row_number.size(); i++)
    std::cout << row_number[i] << ", ";
  std::cout << std::endl << std::endl << "point: ";
  for (int i = 0; i < point.size(); i++)
    std::cout << point[i] << ", ";
  std::cout << std::endl << std::endl;
  SparceMatrix B = this->Transpose();
  int iter=0;
  for (int i = 0; i < B.nCol; i++) {
    int k;
    if (i==0)
      k=B.point[0];
    else if(i == B.nCol-1)
      k = B.val.size();
    else
      k = B.point[i];
    for (int i = 0; i < B.nRow; i++) {
      bool exist = false;
      if (iter<k) {
        if (B.row_number[iter] == i) {
          std::cout << B.val[iter] << "\t";
          iter++;
          exist = true;
        }
      }
      if (exist == false)
        std::cout << "0\t\t";
    }
    std::cout<<std::endl;
  }
  std::cout << std::endl;
}

int SparceMatrix::colCount(int col) {
  if (col == 0)
    return point[0];
  else if (col == nCol - 1)
    return val.size() - point[nCol-2];
  else
    return point[col] - point[col - 1];
}