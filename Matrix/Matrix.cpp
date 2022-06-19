#ifndef MATRIX_CPP
#define MATRIX_CPP
#include "omp.h"
#include "Matrix.h"

// Default Constructor
template <typename T>
Matrix<T>::Matrix() : m_rows(1), m_cols(1)
{
  mat.resize(1);
}

// Parameter Constructor
template <typename T>
Matrix<T>::Matrix(unsigned rows, unsigned cols, const int &initial)
{
  mat.resize(rows);

  for (auto i=0; i<static_cast<int>(mat.size()); i++)
        mat[i].resize(cols, initial);


  m_rows = rows;
  m_cols = cols;
}

// Copy Constructor

template <typename T>
Matrix<T>::Matrix(const Matrix<T> &rhs)
{
  mat = rhs.mat;
  m_rows = rhs.get_rows();
  m_cols = rhs.get_cols();
}

// Assignment Operator

// element by element copy
template <typename T>
Matrix<T> &Matrix<T>::operator=(const Matrix<T> &rhs)
{
  if (&rhs == this)
    return *this;

  auto new_rows = static_cast<int>(rhs.get_rows());
  auto new_cols = static_cast<int>(rhs.get_cols());

  mat.resize(new_rows);

  for (auto i = 0; i < static_cast<int>(mat.size()); i++)
    mat[i].resize(new_cols);

  std::lock_guard<std::mutex> lockGuard(m_mutex);
  for (auto i = 0; i < new_rows; i++)
    for (auto j = 0; j < new_cols; j++)
      mat[i][j] = rhs(i, j);

  m_rows = new_rows;
  m_cols = new_cols;

  return *this;
}

// Calculate a transpose of this matrix
template<typename T>
Matrix<T> Matrix<T>::transpose()
{

  Matrix result(m_rows, m_rows, 0.0);

  for (unsigned i=0; i<m_rows; i++)
    for (unsigned j=0; j<m_rows; j++)
      result(i,j) = this->mat[j][i];

  return result;
}

template <typename T>
Matrix<T> Matrix<T>::operator*(const Matrix<T> &rhs)
{
  auto rows = static_cast<int>(rhs.get_rows());
  auto cols = static_cast<int>(rhs.get_cols());
  // declare result matrix
  Matrix result(rows, cols, 0.0);
  // mutex lock to prevent race conditions
  std::lock_guard<std::mutex> lockGuard(m_mutex);
  auto i = 0, j = 0, k = 0;

//#pragma omp parallel for num_threads(2) collapse(2) private(i, j, k) shared(result)
  for (i = 0; i < rows; i++)
    for (j = 0; j < cols; j++)
      for (k = 0; k < rows; k++)
        result(i, j) += this->mat[i][k] * rhs(k, j);

  return result;
}


// function makes use of assignment operator and * operator above

template <typename T>
Matrix<T> &Matrix<T>::operator*=(const Matrix<T> &rhs)
{
  Matrix result = (*this) * rhs;
  (*this) = result;

  return *this;
}

// Access  individual elements
template <typename T>
T &Matrix<T>::operator()(const unsigned &row, const unsigned &col)
{
  return this->mat[row][col];
}

// Access  individual elements (const)
template <typename T>
const T &Matrix<T>::operator()(const unsigned &row, const unsigned &col) const
{
  return this->mat[row][col];
}

// Access  individual vectors
template <typename T>
std::vector<T> &Matrix<T>::operator[](const unsigned &row)
{
  return this->mat[row];
}

// Access  individual vectors (const)
template <typename T>
const std::vector<T> &Matrix<T>::operator[](const unsigned &row) const
{
  return this->mat[row];
}

// Get number of rows of matrix
template <typename T>
unsigned Matrix<T>::get_rows() const
{
  return this->m_rows;
}

// Get number of columns of  matrix
template <typename T>
unsigned Matrix<T>::get_cols() const
{
  return this->m_cols;
}
#endif
