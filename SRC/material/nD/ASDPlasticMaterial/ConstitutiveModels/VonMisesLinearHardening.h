/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// Original implementation: José Abell (UANDES), Massimo Petracca (ASDEA)
//
// ASDPlasticMaterial
//
// Fully general templated material class for plasticity modeling


#include "../ASDPlasticMaterial.h"



#include <classTags.h>
// New materials are created by subclassing instances of the ASDPlasticMaterial<.,.,.,.,>
// template class, with the appropriate components as template parameters.
// Heavy use of templating is made, therefore typedeffing is a friend in helping clear up the mess.



//Von Mises Model with linear hardening (VMLH)
class VonMisesLinearHardening;  //This model we will define

//Typedefs for internal variables list, yield function, and plastic flow function
// typedef MaterialInternalVariables < LinearHardeningTensor_EV, LinearHardeningScalar_EV> VMLHVarsType;
// typedef VonMises_YF < LinearHardeningTensor_EV, LinearHardeningScalar_EV> VMLH_YFType;
// typedef VonMises_PF < LinearHardeningTensor_EV, LinearHardeningScalar_EV> VMLH_PFType;


using VMSL = VonMisesRadiusIV<ScalarLinearHardeningFunction>;
using BSTL = BackStressIV<TensorLinearHardeningFunction>;
using VMLH_YFType = VonMises_YF<BSTL, VMSL>;
using VMLH_PFType = VonMises_PF<BSTL, VMSL>;

using VMLHBase = ASDPlasticMaterial <LinearIsotropic3D_EL,
        VMLH_YFType,
        VMLH_PFType,
        ND_TAG_ASDPlasticMaterial,
        VonMisesLinearHardening > ;




//Define the new class. We must provide two constructor and the evolving variables as data-members.
class VonMisesLinearHardening : public VMLHBase
{
public:
    VonMisesLinearHardening(int tag_in, double rho_) :
        VMLHBase::ASDPlasticMaterial(tag_in,rho_) 
        {
        	VMSL vmsl(1);
        	BSTL bstl(VoigtVector(0,0,0,0,0,0)); 

        	iv_storage.set(vmsl);
        	iv_storage.set(bstl);

		    YoungsModulus E(1000);
		    PoissonsRatio nu(0.25);
			ScalarLinearHardeningParameter HS(2.0);
			TensorLinearHardeningParameter HT(1.0);
			
		    parameters_storage.set(E);
		    parameters_storage.set(nu);
		    parameters_storage.set(HT);
		    parameters_storage.set(HS);

		    cout << "ASDPlasticMaterial" << endl;
	        cout << "  Yield Function          : " << yf.NAME << endl;
	        cout << "  Plastic flow direction  : " << pf.NAME << endl;
	        cout << "  Elasticity              : " << et.NAME << endl;
	        cout << "  # of Internal variables :" << iv_storage.size() <<  endl;
	        iv_storage.print_components();
	        cout << "  # of Parameters         :" << parameters_storage.size() <<  endl;
	        parameters_storage.print_components();
        }

    // VonMisesLinearHardening(int tag_in, double k0_in, double H_alpha, double H_k, double E, double nu, double rho_) :
    //     VMLHBase::ASDPlasticMaterial(tag_in, rho_, 0.0, //Initial confinement can be 0 for this model
    //                                  VMLH_YFType(alpha, k),       // Point YF to internal variables
    //                                  LinearIsotropic3D_EL(E, nu), // Create Elasticity
    //                                  VMLH_PFType(alpha, k),       // Point PF to the internal variables
    //                                  VMLHVarsType(alpha, k)),     // Declare the list of internal variables
    //     alpha(H_alpha),
    //     k(H_k, k0_in)
    // {
    //     cout << "First VMLH ctor" << endl;
    //     cout << "tag_in  = " << tag_in << endl;
    //     cout << "k0_in  = " << k0_in << endl;
    //     cout << "H_alpha  = " << H_alpha << endl;
    //     cout << "H_k  = " << H_k << endl;
    //     cout << "E  = " << E << endl;
    //     cout << "nu  = " << nu << endl;
    //     cout << "rho_  = " << rho_ << endl;
    //     cout << "yf  = " << yf(getStressTensor()) << endl;

    // }
    
    // VonMisesLinearHardening(int tag_in, double k0_in, double H_alpha, double H_k, double E, double nu, double rho_) :
    //     VMLHBase::ASDPlasticMaterial(tag_in, rho_, 0.0, //Initial confinement can be 0 for this model
    //                                  VMLH_YFType(alpha, k),       // Point YF to internal variables
    //                                  LinearIsotropic3D_EL(E, nu), // Create Elasticity
    //                                  VMLH_PFType(alpha, k),       // Point PF to the internal variables
    //                                  VMLHVarsType(alpha, k)),     // Declare the list of internal variables
    //     alpha(H_alpha),
    //     k(H_k, k0_in)
    // {
    //     // cout << "First VMLH ctor" << endl;
    //     // cout << "tag_in  = " << tag_in << endl;
    //     // cout << "k0_in  = " << k0_in << endl;
    //     // cout << "H_alpha  = " << H_alpha << endl;
    //     // cout << "H_k  = " << H_k << endl;
    //     // cout << "E  = " << E << endl;
    //     // cout << "nu  = " << nu << endl;
    //     // cout << "rho_  = " << rho_ << endl;
    //     // cout << "yf  = " << yf(getStressTensor()) << endl;

    // }

    // Second constructor is not called by the user, instead it is called when creating a copy of the
    // material. This must provide an initialization for the state variables and link the components
    // to these variables appropriately.
    // VonMisesLinearHardening(int tag_in, double rho, double p0, VMLH_YFType &yf,
    //                         LinearIsotropic3D_EL &el,
    //                         VMLH_PFType &pf,
    //                         VMLHVarsType &vars) :
    //     VMLHBase::ASDPlasticMaterial(tag_in, this->getRho(), p0, // Initial confinement can be 0 for this model
    //                                  VMLH_YFType(alpha, k),     // Point YF to new internal variables
    //                                  LinearIsotropic3D_EL(el),  // Create Elasticity -- use copy constructor here
    //                                  VMLH_PFType(alpha, k),     // Point PF to the internal variables
    //                                  VMLHVarsType(alpha, k)),   // Declare the list of internal variables
    //     alpha(0),
    //     k(0, 0)
    // {
    // }

    // VonMisesLinearHardening() :
    //     VMLHBase::ASDPlasticMaterial(0, 0, 0.0, //Initial confinement can be 0 for this model
    //                                  VMLH_YFType(alpha, k),       // Point YF to internal variables
    //                                  LinearIsotropic3D_EL(0, 0),  // Create Elasticity
    //                                  VMLH_PFType(alpha, k),       // Point PF to the internal variables
    //                                  VMLHVarsType(alpha, k)),     // Declare the list of internal variables
    //     alpha(0),
    //     k(0, 0)
    // {}

    void Print(OPS_Stream& s, int flag=0) {
        s << "VonMisesLinearHardening::" << endln;
        s << "\tTag: " << this->getTag() << endln;
        s << " Please Implement Me !!! " << endln;
        // s << "\tElastic_Modulus: " << E << endln;
        // s << "\tPoissons_Ratio: " << v << endln;
        // s << "\tDensity: " << rho << endln;
        // s << "\tVon_Mises_radius: "
        // s << "\tKinematic_hardening_rate: "
        // s << "\tIsotropic_hardening_rate: "
    }


};

