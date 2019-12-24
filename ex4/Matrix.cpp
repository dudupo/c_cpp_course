#include "Matrix.h"


Matrix::Matrix(int rows, int cols) : rows(rows), cols(cols)
{

        this->matrix = (float **) malloc( sizeof(float * ) * rows);
        for (int i = 0; i < rows; i++)
        {
                this->matrix[i] = (float *) malloc( sizeof(float) * cols);
        }

        LAMBDA( fun ) {
                UNUSED_LAMBDA();
                s = 0.f;
        };

        this->forEach(fun, nullptr);
}
Matrix::Matrix() : Matrix(0,0)
{

}

Matrix::Matrix( const Matrix& other ) : Matrix(other.getRows(), other.getCols())
{

        LAMBDA( fun ) {
                s = float( (*(Matrix *) args)(i,j) );
        };

        void * args = ( void * ) &other;
        this->forEach(fun, args);
}

void Matrix::freeMatrix( ) {
        for (int i = 0; i < this->getRows(); i++)
        {
                delete[] this->matrix[i];
        }
        delete [] this->matrix;
}

Matrix::~Matrix()
{
        this->freeMatrix();
}
int Matrix::getRows() const {
        return this->rows;
}
int Matrix::getCols() const {
        return this->cols;
}

Matrix& Matrix::vectorize()
{
        float ** new_matrix = new float* [ this->getRows() * this->getCols() ];

        for (int i = 0; i < this->getRows() * this->getCols(); i++ )
        {
                new_matrix[i] = new float[1];
                new_matrix[i][0] = (*this)[i];
        }

        this->freeMatrix();
        this->matrix = new_matrix;

        this->rows =  this->getRows() * this->getCols();
        this->cols = 1;

        return *this;
}
Matrix & Matrix::operator= ( const Matrix &assignmented ) {
        std::cout << " i was here " << '\n';


        this->matrix = (float **) malloc( sizeof(float * ) * assignmented.getRows() );  //new float [this->getRows()][this->getCols()];
        for (int i = 0; i < assignmented.getRows(); i++)
        {
                this->matrix[i] = (float *) malloc( sizeof(float) * assignmented.getCols());
        }

        this->rows = assignmented.getRows();
        this->cols = assignmented.getCols();


        LAMBDA( fun ) {
                s = float( (*(Matrix *) args)(i,j) ); //(i,j);
        };

        void * args = ( void * ) &assignmented;
        this->forEach(fun, args);
        //
        // this = &assignmented;



        return *this;
}
Matrix& Matrix::operator+ ( const Matrix& other ) {
        Matrix& ref = *( new Matrix(  this->getRows(), this->getCols() ));

        LAMBDA(fun) {
                Matrix ** ptr = ( Matrix ** ) args;
                s = (*ptr[0])(i,j) + (*ptr[1])(i,j);
        };

        void * args[2] = { (void *) this, (void *) &other};
        ref.forEach(fun, args);
        return ref;
}
void Matrix::operator+=  ( const Matrix& other ) {
        LAMBDA(fun) {
                Matrix * ptr = ( Matrix * ) args;
                s += (*ptr)(i,j);
        };
        void * args =  (void *) &other;
        this->forEach(fun, args);

}
Matrix& Matrix::operator* (  const Matrix& other ) {
        Matrix * ref = new Matrix(  this->getRows(), other.getCols() );

        std::cout << ref->getRows() << '\n';
        std::cout << ref->getCols() << '\n';

        std::cout << (*this)(0,324) << '\n';
        LAMBDA(fun) {
                UNUSED_LAMBDA();
                Matrix ** ptr = ( Matrix ** ) args;

                for ( int k = 0; k < (ptr[0])->getCols(); k++ )
                {
                        s += (*ptr[0])(i,k) * (*ptr[1])(k,j);
                }
        };

        void * args[2] = { (void *) this, (void *) &other};
        ref->forEach(fun, args);
        return *ref;
}
void Matrix::ones( ) {
        LAMBDA(fun){
                UNUSED_LAMBDA();
                s = 1.f;
        };

        this->forEach( fun,  nullptr );
}
Matrix& Matrix::operator* ( float scalar ) const {
        Matrix& ref =  *(new Matrix(  *this ));
        LAMBDA(fun) {
                UNUSED_LAMBDA();
                s *=  (*(float *)args);
        };
        ref.forEach(fun,  (void *)&scalar );
        return ref;
}

typedef struct pair_col_os {
        std::ostream * os;
        int cols;

} pair_col_os;

void Matrix::plainPrint() {
        LAMBDA(fun) {
                UNUSED_LAMBDA();

                pair_col_os * ptr = ( pair_col_os *) args;
                *(ptr->os) << s << " ";

                if ( j+1 == ptr->cols  )
                {
                        *(ptr->os) << "\n";
                }
        };

        pair_col_os * args =
                ( pair_col_os *) malloc( sizeof( pair_col_os ));

        args->os = &std::cout;
        args->cols = this->getCols();
        this->forEach(fun, (void *) args);
        *(args->os) << std::endl;
        free(args);


}
std::ostream& operator<< ( std::ostream& os, Matrix& matrix ) {

        LAMBDA(fun) {
                UNUSED_LAMBDA();
                pair_col_os * ptr = ( pair_col_os *) args;
                if ( s > 0.1f )
                {
                        *(ptr->os) << "**";
                }
                else
                {
                        *(ptr->os) << "  ";
                }
                if ( j+1 == ptr->cols  )
                {
                        *(ptr->os) << "\n";
                }
        };


        pair_col_os * args =
                ( pair_col_os *) malloc( sizeof( pair_col_os ));

        args->os = &os;
        args->cols = matrix.getCols();

        matrix.forEach(fun, (void *) args);
        free(args);
        os << std::endl;
        return os;
}
float Matrix::operator()  ( const int i, const int j ) {
        return this->matrix[i][j];
}
float Matrix::operator[]  ( const int i ) {
        return this->matrix[i / this->getCols()][ i % this->getCols() ];
}
Matrix& operator* (float scalar, const Matrix& other ){
        return other * scalar;
}


// initillaize the matrix by given file stream.
int operator>>( std::ifstream& is, Matrix& matrix ) {
        std::cout << "\n" << sizeof(float) * matrix.getCols() *  matrix.getRows()   << '\n';

        for (int i = 0; i < matrix.getRows(); i++)
        {
                for (int j = 0; j < matrix.getCols(); j++)
                {
                        //is >> matrix.matrix[i][j];
                        is.seekg(sizeof(float) * (  i * matrix.getCols()  + j ) );
                        is.read(reinterpret_cast<char *>(
                                &matrix.matrix[i][j]),sizeof(float) );

                }
        }

        std::cout << is.eof() << " " <<  is.fail()  <<" "<< is.bad() << std::endl;
        std::cout << is.good()  << "\n";
        return 1;
}


void Matrix::forEach( void (*fun)( float&, void*, int, int), void *args )
{
        for (int i = 0; i < this->getRows(); i++)
        {
                for (int j = 0; j < this->getCols(); j++)
                {
                        fun(this->matrix[i][j], args, i, j);
                }
        }
}
