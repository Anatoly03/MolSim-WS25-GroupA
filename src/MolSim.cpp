
#include <iostream>
#include <list>

#include "FileReader.h"
//#include "outputWriter/XYZWriter.h"
#include "utils/ArrayUtils.h"

/**** forward declaration of the calculation functions ****/

/**
 * calculate the force for all particles
 */
void calculateF();

/**
 * calculate the position for all particles
 */
void calculateX();

/**
 * calculate the position for all particles
 */
void calculateV();



/**
 * plot the particles to a xyz-file
 */
void plotParticles(int iteration);

/**
 * helper function that add 2 double vector together
 */
auto addVectorDouble(const std::array<double, 3>& firstVector, const std::array<double, 3>& secondVector) {
  std :: array<double , 3> resultVector={0.,0.,0.};
  for (int i = 0; i < firstVector.size(); ++i) {
    resultVector[i]=firstVector[i]+secondVector[i];
  };

  return resultVector;

}


/**
 * helper function that subtract 2 double vector
 */
auto subtractVectorDouble(const std::array<double, 3>& firstVector, const std::array<double, 3>& secondVector) {
  std :: array<double , 3> resultVector={0.,0.,0.};
  for (int i = 0; i < firstVector.size(); ++i) {
    resultVector[i]=firstVector[i]-secondVector[i];
  };

  return resultVector;

}

constexpr double start_time = 0;
constexpr double end_time = 1000;
constexpr double delta_t = 0.014;

// TODO: what data structure to pick?
std::list<Particle> particles;

int main(int argc, char *argsv[]) {
  std::cout << "Hello from MolSim for PSE!" << std::endl;
/**
 * The code below here is for testing the 3 calculation
 *
 * constexpr double start_time = 0;
  constexpr double end_time = 1000;
  constexpr double delta_t = 0.014;
 */
  std::array<double, 3> x_arg1={1.,0.,2.};
  std::array<double, 3> v_arg1={0.,1.,2.};
  double m_arg1=2;
  int type_arg1=0;

  std::array<double, 3> x_arg2={1.,1.,2.};
  std::array<double, 3> v_arg2={1.,1.,2.};
  double m_arg2=2;
  int type_arg2=0;

  std::array<double, 3> x_arg3={3.,0.,2.};
  std::array<double, 3> v_arg3={1.,2.,2.};
  double m_arg3=3;
  int type_arg3=0;

  Particle testP1(x_arg1,v_arg1,m_arg1,type_arg1);
  Particle testP2(x_arg2,v_arg2,m_arg2,type_arg2);
  Particle testP3(x_arg3,v_arg3,m_arg3,type_arg3);

  particles.push_front(testP1);
  particles.push_front(testP2);
  particles.push_front(testP3);


  calculateF();
  std::cout << " "  << std::endl;
  calculateV();
  std::cout << " "  << std::endl;
  calculateX();
  for (auto &p : particles) {
    std::cout << "partical " << p << " with new f" << std::endl;

  }

  /**
   * end of testing codes
   */





  if (argc != 2) {
    std::cout << "Erroneous programme call! " << std::endl;
    std::cout << "./molsym filename" << std::endl;
  }

  FileReader fileReader;
  fileReader.readFile(particles, argsv[1]);

  double current_time = start_time;

  int iteration = 0;

  // for this loop, we assume: current x, current f and current v are known
  while (current_time < end_time) {
    // calculate new x
    calculateX();
    // calculate new f
    calculateF();
    // calculate new v
    calculateV();



    iteration++;
    if (iteration % 10 == 0) {
      plotParticles(iteration);
    }
    std::cout << "Iteration " << iteration << " finished." << std::endl;

    current_time += delta_t;
  }

  std::cout << "output written. Terminating..." << std::endl;
  return 0;
}

void calculateF() {
  std::list<Particle>::iterator iterator;
  iterator = particles.begin();

  std::cout << "calculating f..."<< std::endl;



  for (auto &p1 : particles) {

    std::array<double, 3> F_i = {0., 0., 0.};
    for (auto &p2 : particles) {
      // @TODO: insert calculation of forces here!
      if(!(p1==p2)) {
        std::cout << "current p1: "<< p1.toString() << std::endl;
        std::cout << "comparing with p: "<< p2.toString() << std::endl;
        std :: array<double , 3> newX{};
        double xijDiffereneceTotal=0;
        double xijDifferenece=0;
        for (int i = 0; i < p1.getX().size(); ++i) {
          newX[i]=p2.getX()[i]-p1.getX()[i];
          xijDifferenece=p1.getX()[i]-p2.getX()[i];
          xijDiffereneceTotal+=xijDifferenece*xijDifferenece;
        }
        xijDiffereneceTotal=std::sqrt(xijDiffereneceTotal);
        std::cout << "xijDiffereneceTotal " << xijDiffereneceTotal << std::endl;
        double tempValue = (p1.getM() * p2.getM())/(xijDiffereneceTotal*xijDiffereneceTotal*xijDiffereneceTotal);
        std::cout << "tempValue " << tempValue << std::endl;
        std::cout<< "Particle difference: X:" << newX[0]  << " , " << newX[1] << " , " << newX[2] << std::endl;
        std :: array<double , 3> F_ij={newX[0]*tempValue,newX[1]*tempValue,newX[2]*tempValue};
        std::cout<< "F_ij: X:" << F_ij[0]  << " , " << F_ij[1] << " , " << F_ij[2] << std::endl;
        F_i= addVectorDouble(F_i,F_ij);
      }
    }
    std::cout << "partical " << p1 << " f finished." << std::endl;
    p1.setNewF(F_i);
    F_i[0]=0;
    F_i[1]=0;
    F_i[2]=0;
    std::cout << "partical " << p1 << " f new finished." << std::endl;
    std::cout << " " << std::endl;
  }

}

void calculateX() {
  std::cout << "calculating x..."<< std::endl;
  for (auto &p : particles) {
    // @TODO: insert calculation of position updates here!

    double coeff=((delta_t*delta_t)/(2*p.getM()));
    std::cout << "coefficient: " << coeff  << std::endl;
    std::array<double, 3> tmp= p.getF();
    std::array<double, 3> tmp2= p.getV();
    for (int i = 0; i < tmp.size(); ++i) {
      std::cout << "tmp["<<i<<"]: " << tmp[i]  << std::endl;
    }
    for (int i = 0; i < tmp.size(); ++i) {
      tmp[i]=coeff*tmp[i];

    }
    for (int i = 0; i < tmp2.size(); ++i) {
      tmp2[i]=delta_t*tmp2[i];

    }

    p.setNewX(addVectorDouble(p.getX(),(addVectorDouble(tmp,tmp2))));
  }
}

void calculateV() {
  std::cout << "calculating v..."<< std::endl;
  for (auto &p : particles) {
    // @TODO: insert calculation of veclocity updates here!

    std::cout << "current p: "<< p.toString() << " m: "<<p.getM()<< std::endl;
    std::cout << "delta t: "<<delta_t<< std::endl;
    double coeff=((delta_t)/(2*p.getM()));
    std::cout << "coefficient: " << coeff  << std::endl;
    std::array<double, 3> tmp= addVectorDouble(p.getOldF(),p.getF());
    for (int i = 0; i < tmp.size(); ++i) {
      std::cout << "tmp["<<i<<"]: " << tmp[i]  << std::endl;
    }
    for (int i = 0; i < tmp.size(); ++i) {
      tmp[i]=coeff*tmp[i];

    }

    p.setNewV(addVectorDouble(p.getV(),tmp));
    std::cout << " "  << std::endl;
  }
}



void plotParticles(int iteration) {
  std::string out_name("MD_vtk");

  //outputWriter::XYZWriter writer;
  //writer.plotParticles(particles, out_name, iteration);
}

