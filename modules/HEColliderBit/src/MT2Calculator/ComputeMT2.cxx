#include <iostream>
#include <math.h>
#include "ComputeMT2.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TError.h"
#include "TMinuit.h"
#include "TMinuitMinimizer.h"
#include "TVector3.h"
#include "TLorentzVector.h"

double ET(double,double);
double MT(double,double,double,double,double);
double NumMT2(const double *xx);
double fAT(TLorentzVector, TLorentzVector);
double MT2(TLorentzVector, TLorentzVector, double, double);

ComputeMT2::ComputeMT2(TLorentzVector visa, TLorentzVector visb, TLorentzVector MET, double ma, double mb, TLorentzVector Up) {
  V1 = visa;
  V2 = visb;
  ET = MET;
  Ma = ma;
  Mb = mb;
  U = Up;
  if (U.E() > 0.){
    V1 = perpify(V1,U);
    V2 = perpify(V2,U);
    ET = perpify(ET,U);
  }
}
ComputeMT2::~ComputeMT2() {
}
TLorentzVector ComputeMT2::perpify(TLorentzVector V, TLorentzVector P){

  TVector3 V3 = TVector3(V.Px(),V.Py(),0.);
  TVector3 P3 = TVector3(P.Px(),P.Py(),0.);
  TVector3 hold1 = V3.Cross(P3);
  hold1 = P3.Cross(hold1);
  hold1 = hold1*(1./(P3.Mag()*P3.Mag()));
  TLorentzVector out;
  out.SetXYZM(hold1.Px(),hold1.Py(),0.,V.M());

  return out;
}
double ComputeMT2::ComputeAnalytic() const {

    TLorentzVector a = V1;
    TLorentzVector b = V2;

    double M2p = 0.5*(Mb*Mb+Ma*Ma);
    double M2m = 0.5*(Mb*Mb-Ma*Ma);

    TVector3 Q = -(a.Vect()+b.Vect()+0.);
    double Q2 = Q.Px()*Q.Px()+Q.Py()*Q.Py();
    double parta = Q2+(Ma+Mb)*(Ma+Mb);
    parta = sqrt(std::max(0.,parta));

    double e12 = a.M()*a.M()+a.Pt()*a.Pt();
    double e1 = std::max(e12,0.);
    e1 = sqrt(e1);

    double e22 = b.M()*b.M()+b.Pt()*b.Pt();
    double e2 = std::max(e22,0.);
    e2 = sqrt(e2);

    //First, we check balanced versus unbalanced.

    TVector3 qt0a = (Ma/a.M())*a.Vect();
    TVector3 qt0b = (Mb/b.M())*b.Vect();
    TVector3 qtb = -qt0a+Q;
    TVector3 qta = -qt0b+Q;

    double eatild2 = Ma*Ma+qta.X()*qta.X()+qta.Y()*qta.Y();
    double eatild = std::max(eatild2,0.);
    eatild = sqrt(eatild);

    double ebtild2 = Mb*Mb+qtb.X()*qtb.X()+qtb.Y()*qtb.Y();
    double ebtild = std::max(ebtild2,0.);
    ebtild = sqrt(std::max(0.,ebtild));

    double MTa2 = a.M()*a.M()+Ma*Ma+2.*(e1*eatild-a.Px()*qta.Px()-a.Py()*qta.Py());
    double MTb2 = b.M()*b.M()+Mb*Mb+2.*(e2*ebtild-b.Px()*qtb.Px()-b.Py()*qtb.Py());

    double MTa = sqrt(std::max(0.,MTa2));
    double MTb = sqrt(std::max(0.,MTb2));

    if ((MTa > b.M()+Mb) && (MTb > a.M()+Ma)){ //balanced

      double AT = fAT(a,b);
      double ma2 = a.M2();
      double mb2 = b.M2();
      double commondenom = 2.*AT-ma2-mb2;

      double sqrttermA = 4.*M2p/commondenom;
      double sqrttermB = 2.*M2m/commondenom;
      sqrttermB = sqrttermB*sqrttermB;
      double sqrtterm = 1.+sqrttermA+sqrttermB;
      sqrtterm = sqrt(std::max(0.,sqrtterm));

      double sqrtfact = sqrt(std::max(0.,AT*AT-ma2*mb2));

      //Let's figure out the ambiguity in sign.

      double sqrtsminhat = e1+e2+parta; //sqrt(\hat{s}_T)_{min}

      double sterma = 2.*(e2 - e1)*M2m/commondenom;
      double stermb = ((e2+e1)*AT-(e2*ma2+e1*mb2))/sqrtfact;

      double shatM = e1+e2+sterma-stermb*sqrtterm;

      double sign = -1.;

      if (shatM < sqrtsminhat){
	sign = 1.;
      }

      //Now, back to MT2

      double MT2term1 = M2m*(mb2-ma2)/commondenom;
      double MT2sq = M2p+AT+MT2term1+sign*sqrtterm*sqrtfact;
      double MT2 = sqrt(std::max(0.,MT2sq));

      return MT2;

    }

    if ((MTa > b.M()+Mb)){ //unbalanced

      return a.M()+Ma;

    }

    if ((MTb > a.M()+Ma)){ //unbalanced

      return b.M()+Mb;
      
    }
    return -1.;
  
}
double ComputeMT2::ComputeNumeric() const {
  const char *minName = "Minuit2";
  const char *algoName = "Combined"; 

  ROOT::Math::Minimizer* mini = ROOT::Math::Factory::CreateMinimizer(minName, algoName);

  mini->SetMaxFunctionCalls(1000000); 
  mini->SetTolerance(0.001);
  mini->SetPrintLevel(0);
  
  ROOT::Math::Functor f(&NumMT2,12);

  mini->SetFunction(f);

  mini->SetFixedVariable(0,"ma",V1.M());
  mini->SetFixedVariable(1,"Ma",Ma);
  mini->SetFixedVariable(2,"mb",V2.M());
  mini->SetFixedVariable(3,"Mb",Mb);
  mini->SetFixedVariable(4,"pxa",V1.Px());
  mini->SetFixedVariable(5,"pya",V1.Py());
  mini->SetFixedVariable(6,"pxb",V2.Px());
  mini->SetFixedVariable(7,"pyb",V2.Py());
  mini->SetFixedVariable(8,"Ex",ET.Px());
  mini->SetFixedVariable(9,"Ey",ET.Py());
  mini->SetVariable(10,"qx",ET.Px()/2., 1.);
  mini->SetVariable(11,"qy",ET.Py()/2., 1.);
  mini->Minimize();

  return mini->MinValue();

}

double ET(double m, double p){

  double out = std::max(0.,m*m+p*p);
  return sqrt(out);

}

double MT(double m, double M, double p, double q, double sign){

  double em = ET(m,p);
  double eM = ET(M,q);
  double out = std::max(0.,m*m+M*M+2.*(em*eM-p*q*sign));
  return sqrt(out);
}

double fAT(TLorentzVector vis1, TLorentzVector vis2){

  double e12 = vis1.M2()+vis1.Pt()*vis1.Pt();
  double e1 = std::max(e12,0.);
  e1 = sqrt(e1);

  double e22 = vis2.M2()+vis2.Pt()*vis2.Pt();
  double e2 = std::max(e22,0.);
  e2 = sqrt(e2);

  double dotprod = vis1.Px()*vis2.Px()+vis1.Py()*vis2.Py();

  return e1*e2+dotprod;

}

double NumMT2(const double *xx )
{
  double ma = xx[0];
  double Ma = xx[1];
  double mb = xx[2];
  double Mb = xx[3];
  double pxa = xx[4];
  double pya = xx[5];
  double pxb = xx[6];
  double pyb = xx[7];
  double Ex = xx[8];
  double Ey = xx[9];
  double qx = xx[10];
  double qy = xx[11];
  double qa = sqrt(std::max(0.,qx*qx+qy*qy));
  double pa = sqrt(std::max(0.,pxa*pxa+pya*pya));
  double par1 = (pxa*qx+pya*qy)/(qa*pa);
  double mta = MT(ma,Ma,pa,qa,par1);
  double qb = sqrt(std::max(0.,(Ex-qx)*(Ex-qx)+(Ey-qy)*(Ey-qy)));
  double pb = sqrt(std::max(0.,pxb*pxb+pyb*pyb));
  double par2 =(pxb*(Ex-qx)+pyb*(Ey-qy))/(qb*pb);
  double mtb = MT(mb,Mb,pb,qb,par2);
  double val = std::max(mta,mtb);

  return val;
}
