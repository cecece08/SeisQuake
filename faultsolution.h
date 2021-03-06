//-----------------------------------------------------------------------------
// Source: faulsolution.h
// Module: focimt
// Class for storing fault plane solution data.
//
// Copyright (c) 2013-2017, Grzegorz Kwiatek.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------
#ifndef faultsolutionH
#define faultsolutionH
//---------------------------------------------------------------------------
#include "moment_tensor.h"

namespace Taquart {
  //! Seismic moment tensor solution data structure.
  /*! This class the output data from the usmt.exe application -
   *  one of the solution (full, trace-null or double-couple) from the
   *  moment tensor inversion in the time domain. It also
   *  wraps the input/output routines for reading and writing the solution
   *  from/to the INI file.
   *  \ingroup foci
   */
  class FaultSolution {
    public:
      // Public data members

      //! Seismic moment tensor
      /*! This parameter keeps the moment tensor matrix. The M[1][1] element
       *  corresponds to M11 tensor element, M[1][2] to M12 and so on. The
       *  values are in Nm.
       */
      double M[4][4]; /*!< Moment tensor components. Elements M[0][*] and M[*][0] are not used. */
      double T0; /*!< Rupture time in seconds. */
      double M0; /*!< Scalar moment tensor value in Nm. */
      double MT; /*!< Total seismic moment tensor value in Nm. */
      double ERR; /*!< Maximum error of the scalar seismic moment tensor
       value in Nm. This value is a square root of the maximum
       element for the moment tensor solution covariance
       matrix. */
      double EXPL; /*!< The size of the explosive component in the
       seismic moment tensor, in percents. */
      double CLVD; /*!< The size of the CLVD, compensated linear vector
       dipole component in the seismic moment tensor,
       in percents. */
      double DBCP; /*!< The size of the shear, double-couple component
       in the seismic moment tensor, in percents. */
      double EXPL_VAC;
      double CLVD_VAC;
      double DBCP_VAC;
      double FIA; /*!< Strike of the first fault plane in degrees. */
      double DLA; /*!< Dip of the first fault plane in degrees. */
      double RAKEA; /*!< Rake of the first fault plane in degrees. */
      double FIB; /*!< Strike of the second fault plane in degrees. */
      double DLB; /*!< Dip of the second fault plane in degrees. */
      double RAKEB; /*!< Rake of the second fault plane in degrees. */
      double PXTR; /*!< P-axis trend in degrees. */
      double PXPL; /*!< P-axis plunge in degrees.*/
      double PXAM; /*!< P-axis plunge in degrees.*/
      double TXTR; /*!< T-axis trend in degrees. */
      double TXPL; /*!< T-axis plunge in degrees.*/
      double TXAM; /*!< T-axis plunge in degrees.*/
      double BXTR; /*!< B-axis trend in degrees. */
      double BXPL; /*!< B-axis plunge in degrees.*/
      double BXAM; /*!< B-axis plunge in degrees.*/
      double QI; /*!< Quality factor. This value depends on the special
       settings in FOCI, thus you should take it into
       account while comparing the sesimic moment tensor solutions*/
      double MAGN; /*!< Moment magnitude, calculated by the standard relationships. */
      Taquart::String Type; /*!< Fault type: can be 'Normal fault', 'Reverse fault' or a 'Strike fault' fault. */
      double Covariance[7][7]; /*!< Covariance matrix.*/
      //std::vector<double> U_th;
      //std::vector<double> U_measured;
      double U_th[FOCIMT_MAXCHANNEL];
      double U_measured[FOCIMT_MAXCHANNEL];
      Taquart::String Station[FOCIMT_MAXCHANNEL];
      int U_n;
      double UERR;
      double E[3];

      // Public functions

      //! Default constructor.
      FaultSolution(void);

      //! Default destructor.
      ~FaultSolution(void);

      //! Copy constructor.
      /*! \param Source Reference to the source FaultSolution structure to copy.
       */
      FaultSolution(const FaultSolution &Source);

      //! Assignment operator.
      /*! \param Source Reference to the source FaultSolution structure to copy.
       * \return Reference to the current struture (*this).
       */
      FaultSolution &operator=(const FaultSolution &Source);

      //! Assign values from another FaultSolution object.
      /*! \param Source Reference to the source FaultSolution structure to copy.
       */
      void Assign(const FaultSolution &Source);

      //! Save seismic moment tensor solution data into INI file.
      /* \param File Pointer to a \a TMemIniFile object to write data to it.
       * \param SectionName Name of the INI file section to write data to it.
       */
      //void SaveIni(TMemIniFile *const File, String SectionName);
      //! Load seismic moment tensor solution data from INI file.
      /*! \param File Pointer to a \a TMemIniFile object to read data from it.
       *  For a detailed description on \a TMemIniFile class, see the Borland's
       *  Visual Component Library reference.
       *  \param SectionName Name of the INI file section to read data from it.
       */
      //void LoadIni(TMemIniFile *const File, String SectionName);
      //! Generate an output text stringlist.
      /*! This function adds seismic moment tensor solution data to the
       *  stringlist object.
       *  For a detailed description on \a TMemIniFile class, see the Borland's
       *  Visual Component Library reference.
       *  \param Output Pointer to the \a TStringList object to add data to it.
       */
      //void FillOutputStringList(TStringList *Output, bool PolishVersion);
      //! Export class data as XML.
      /*! \param XMLExporter XML exporter class.
       */
      //XMLNode xmlExport(XMLExporter &Exporter, String SectionName="", String Variant="",
      //  String Description="");
      //void FillCovarianceStringList(TStringList *Output);
    private:
      void sincos(double a, double *s, double *c);
      Taquart::String SubString(Taquart::String Line, int Start, int End);

    protected:
  };

  class FaultSolutions {
    public:
      char Type;
      int Channel;
      Taquart::FaultSolution FullSolution;
      Taquart::FaultSolution TraceNullSolution;
      Taquart::FaultSolution DoubleCoupleSolution;
  };
}

//---------------------------------------------------------------------------
#endif

