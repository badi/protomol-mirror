/* -*- c++ -*- */
#ifndef NORMALMODEUTILITIES_H
#define NORMALMODEUTILITIES_H

#include <protomol/type/Vector3DBlock.h>
#include <protomol/topology/GenericTopology.h>

#include <protomol/type/EigenvectorInfo.h>
#include <protomol/integrator/Integrator.h>

namespace ProtoMol {
  class ScalarStructure;

  /**
   *
   * Specific NormalModeUtilities routines
   *
   */
  class NormalModeUtilities {
  public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Types and Enums
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    enum {COMPLIMENT_FORCES = 1};
    enum {GEN_COMP_NOISE = 2};
    enum {NO_NM_FLAGS = 0};

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Constructors, destructors, assignment
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  public:
    NormalModeUtilities();
    NormalModeUtilities(int firstmode, int nummode, Real gamma, int seed,
                        Real temperature);
    //
    virtual ~NormalModeUtilities();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // New methods of class NormalModeUtilities
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  protected:
    void genProjGauss(Vector3DBlock *gaussRandCoord, GenericTopology *myTopo);
    void genProjGaussC(Vector3DBlock *gaussRandCoord,
                       Vector3DBlock *gaussRandCoordm,
                       GenericTopology *myTopo);
    void nmlDrift(Vector3DBlock *myPositions, Vector3DBlock *myVelocities,
                  Real dt, GenericTopology *myTopo);
    bool testRemainingModes();
    int diagHessian(double *eigVecO, double *eigValO, double *hsnhessM);
    void absSort(double *eigVec, double *eigVal, int *eigIndx);
    double calcRayleigh(double *rQ, double *boundRq, double *hsnhessM, int numv,
                        double raylAverage);
    int minimizer(Real peLim, int numloop, bool simpM, bool reDiag,
                  bool nonSubspace, int *forceCalc, Real *lastLambda,
                  ScalarStructure *myEnergies, Vector3DBlock *myPositions,
                  GenericTopology *myTopo);

    virtual void utilityCalculateForces() {};
  private:
  public:
    Vector3DBlock *subspaceForce(Vector3DBlock *force, Vector3DBlock *iPforce);
    Vector3DBlock *subspaceVelocity(Vector3DBlock *force,
                                    Vector3DBlock *iPforce);
    Vector3DBlock *nonSubspaceForce(Vector3DBlock *force,
                                    Vector3DBlock *iPforce);
    Vector3DBlock *nonSubspacePosition(Vector3DBlock *force,
                                       Vector3DBlock *iPforce);
    void subSpaceSift(Vector3DBlock *velocities, Vector3DBlock *forces);
    double *vector3DBlockTOvect(Vector3DBlock *blkDat, double *vecDat);
    Vector3DBlock *vectTOvector3DBlock(double *vecDat, Vector3DBlock *blkDat);
    void initialize(int sz, GenericTopology *myTopo, Vector3DBlock *myForces,
                    int nm_flags);
    virtual void forceProjection();
    void setIntegratorSetPointers(Integrator *integrator, EigenvectorInfo *eipt,
                                  bool eiValid);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Friends of class NormalModeUtilities
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  public:

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // private and public data members
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  public:
    //pointers to eig pointers, used by all integrators in chain
    double **Q;
    double *eigValP;
    unsigned int numEig;
    //molecule constants
    int _N, _3N, _rfM;
    //linear arrays for blas
    double *tmpFX, *tmpC;
    //inputs for projection/integrator
    int firstMode;
    int numMode;
    Real myGamma;
    int mySeed;
    Real myTemp;
    //eig pointers, used if topmost integrator
    Real *mhQu;
    Real maxEigvalu;
    unsigned int numEigvectsu;
    //compliment randoms, compatibility with NormModeInt
    Vector3DBlock tempV3DBlk;
    Vector3DBlock temp2V3DBlk;
    bool genCompNoise;

  protected:
    double *invSqrtMass, *sqrtMass;
    int numSteps;
    Vector3DBlock gaussRandCoord1, gaussRandCoord2, posTemp;
    Vector3DBlock *myForcesP;
    bool complimentForces;
  };
}
#endif