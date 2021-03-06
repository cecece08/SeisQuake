//-----------------------------------------------------------------------------
// Source: moment_tensor.cpp
// Module: FOCIMT
// Main routine.
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
#include "moment_tensor.h"
#include "faultsolution.h"
#include "inputdata.h"
#include "usmtcore.h"
#include "focimtaux.h"
#include "traveltime.h"
//-----------------------------------------------------------------------------

using namespace std;

//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
  try {
    Taquart::String FilenameIn;
    Taquart::String FilenameOut;
    Taquart::String FilenameVelocity;
    unsigned int N = 0;

    srand((unsigned) time(0));

    Options listOpts;
    int switchInt;
    PrepareHelp(listOpts);

    Taquart::String SolutionTypes = "D";
    Taquart::String NormType = "L2";
    Taquart::String Projection = "SL";
    Taquart::String BallContent = "SACD";
    Taquart::String DumpOrder = "";
    Taquart::String OutputFileType = "PNG";
    unsigned int Size = 500;
    bool JacknifeTest = false;
    bool BootstrapTest = false;
    unsigned int BootstrapSamples = 0;
    double BootstrapPercentReverse = 0.00;
    double BootstrapPercentReject = 0.00;
    double BootstrapAmplitudeModifier = 0.00;
    double BootstrapTakeoffModifier = 0.00;
    bool NoiseTest = false;
    bool DrawFaultOnly = false;
    bool DrawFaultsOnly = false;
    bool DrawStationsOnly = false;
    bool VelocityModel = false;
    bool TakeoffRanges = false;
    Taquart::String TakeoffString;
    double AmpFactor = 1.0f;
    unsigned int AmplitudeN = 100;
    unsigned int i1;
    double v1, v2;
    Taquart::String Temp;
    Taquart::String FaultString;
    Taquart::String StationString;
    if (listOpts.parse(argc, argv))
      while ((switchInt = listOpts.cycle()) >= 0) {
        switch (switchInt) {
          case 0:
            FilenameIn =
                Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            break;
          case 1:
            FilenameOut =
                Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            break;
          case 2:
            SolutionTypes =
                Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            break;
          case 3:
            OutputFileType = Taquart::String(
                listOpts.getArgs(switchInt).c_str()).Trim().UpperCase();
            break;
          case 4:
            NormType =
                Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim().UpperCase();
            break;
          case 5:
            Projection =
                Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim().UpperCase();
            break;
          case 6:
            BallContent =
                Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim().UpperCase();
            break;
          case 7:
            DumpOrder =
                Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            break;
          case 8:
            // Use 1D velocity model from a file (forces different formatting of input file)
            VelocityModel = true;
            FilenameVelocity = Taquart::String(
                listOpts.getArgs(switchInt).c_str()).Trim();
            break;
          case 9:
            JacknifeTest = true;
            break;
          case 10:
            NoiseTest = true;
            Temp = Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            if (Temp.Pos("/")) {
              AmpFactor = Temp.SubString(1, Temp.Pos("/") - 1).ToDouble();
              AmplitudeN = Temp.SubString(Temp.Pos("/") + 1, 1000).ToInt();
            }
            else {
              AmpFactor = Temp.ToDouble();
            }
            break;
          case 11:
            // Draw fault plane solutions only.
            DrawFaultOnly = true;
            FaultString =
                Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            break;
          case 12:
            // Draw fault plane solutions only.
            DrawFaultsOnly = true;
            FaultString =
                Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            break;
          case 13:
            // Draw fault plane solutions only.
            DrawStationsOnly = true;
            StationString =
                Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            break;
          case 14: // Option -z (beach ball size)
            Size =
                int(
                    Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim().ToDouble()
                        + 0.5);
            break;
          case 15: // Option -rt (resampling / takeoff)
            BootstrapTest = true;
            Temp = Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            Dispatch2(Temp, v1, v2);
            i1 = (unsigned int) (v1 + 0.5);
            if (i1 > BootstrapSamples)
              BootstrapSamples = i1;
            BootstrapTakeoffModifier = v2;
            break;
          case 16: // Option -rp (resampling / polarity)
            BootstrapTest = true;
            Temp = Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            Dispatch2(Temp, v1, v2);
            i1 = (unsigned int) (v1 + 0.5);
            if (i1 > BootstrapSamples)
              BootstrapSamples = i1;
            BootstrapPercentReverse = v2;
            break;
          case 17: // Option -rr (resampling / station rejection)
            BootstrapTest = true;
            Temp = Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            Dispatch2(Temp, v1, v2);
            i1 = (unsigned int) (v1 + 0.5);
            if (i1 > BootstrapSamples)
              BootstrapSamples = i1;
            BootstrapPercentReject = v2;
            break;
          case 18: // Option -ra (resampling / amplitude modification)
            BootstrapTest = true;
            Temp = Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            Dispatch2(Temp, v1, v2);
            i1 = (unsigned int) (v1 + 0.5);
            if (i1 > BootstrapSamples)
              BootstrapSamples = i1;
            BootstrapAmplitudeModifier = v2;
            break;
          case 19: // Option -mt (1D tomography map)
            // Use 1D velocity model from a file (forces different formatting of input file)
            // Option -m must be also specified.
            TakeoffRanges = true;
            TakeoffString =
                Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            break;
          case 20: // -cn Option -c (set color) format is r/g/b/a or r/g/b
          case 21: // -cs
          case 22: // -cr
          case 23: // -cd
          case 24: // -ct
          case 25: // -cp
          case 26: // -c+
          case 27: // -c-
          case 28: // -cl
            Temp = Taquart::String(listOpts.getArgs(switchInt).c_str()).Trim();
            ColorSelection(Temp, switchInt - 19); // Intepret color string
            break;
          case 29:
            std::cout << "focimt\nrev. 3.3.1 (2017.02.09)\n"
                "(c) 2013-2017 Grzegorz Kwiatek and Patricia Martinez-Garzon"
                << std::endl;
            break;
        }
      }

    if (FilenameIn.Length() == 0 && DrawFaultOnly == false
        && DrawFaultsOnly == false && VelocityModel == false
        && DrawStationsOnly == false) {
      std::cout << "You must provide a valid filename." << std::endl;
    }

    if (FilenameOut.Length() == 0 && DrawFaultOnly == false
        && DrawFaultsOnly == false && DrawStationsOnly == false) {
      //FilenameOut = Taquart::ExtractFileName(FilenameIn);
      //if (FilenameOut.Pos("."))
      //  FilenameOut = FilenameOut.SubString(1, FilenameOut.Pos(".") - 1);
    }
    else if (FilenameOut.Length() == 0
        && (DrawFaultOnly == true || DrawFaultsOnly == true
            || DrawStationsOnly == true)) {
      FilenameOut = "beachball";
    }

    //---- Read velocity model if necessary.
    std::vector<double> Top;
    std::vector<double> Velocity;
    if (VelocityModel) {
      std::ifstream VelocityFile;
      int n;
      double v;
      VelocityFile.open(FilenameVelocity.c_str());
      VelocityFile >> n;
      for (int i = 0; i < n; i++) {
        VelocityFile >> v;
        Top.push_back(v);
      }
      for (int i = 0; i < n; i++) {
        VelocityFile >> v;
        Velocity.push_back(v);
      }

      // If option -mt is on, get ranges for azimuths and takeoff and
      // output data to a text file.
      if (TakeoffRanges) {
        if (FilenameOut.Length() == 0)
          FilenameOut = "raytracing.txt";
        double dstart, dstep, dend;
        double estart, estep, eend;
        String2MT(TakeoffString, dstart, dstep, dend, estart, estep, eend);
        ofstream OutFile(FilenameOut.c_str(), std::ofstream::out);
        for (double depth = dstart; depth <= dend; depth += dstep) {
          for (double delta = estart; delta <= eend; delta += estep) {
            double traveltime = 0.0f;
            double ray_dist = 0.0f;
            bool directphase = false;
            double takeoff = 0.0f, aoi = 0.0f, sta_elev = 0.0f;
            int kk = 0;

            CalcTravelTime1D_2(sta_elev, depth, delta, Top, Velocity,
                traveltime, takeoff, directphase, aoi, kk, ray_dist);

            OutFile << sta_elev << " ";
            OutFile << depth << " ";
            OutFile << delta << " ";
            OutFile << traveltime << " ";
            OutFile << ((directphase == true) ? '1' : '0') << " ";
            OutFile << takeoff << " ";
            OutFile << aoi << " ";
            OutFile << kk << " ";
            OutFile << ray_dist << std::endl;
          }
        }
        OutFile.close();
        return 0;
      }

      // Try to read one more variable from the velocity model file. If the
      // variable is a string containing "DATA", calculate the raytracing
      // parameters for following data and exit program.
      char data[255];
      VelocityFile >> data;
      Taquart::String datas(data);
      if (datas == "DATA") {
        double traveltime = 0.0f;
        double ray_dist = 0.0f;
        bool directphase = false;
        double takeoff = 0.0f, aoi = 0.0f, depth = 0.0f, delta = 0.0f,
            sta_elev = 0.0f;
        int kk = 0;
        ofstream OutFile(FilenameOut.c_str(), std::ofstream::out);
        while (VelocityFile.good()) {
          VelocityFile >> sta_elev;
          VelocityFile >> depth;
          VelocityFile >> delta;

          CalcTravelTime1D_2(sta_elev, depth, delta, Top, Velocity, traveltime,
              takeoff, directphase, aoi, kk, ray_dist);

          OutFile << sta_elev << " ";
          OutFile << depth << " ";
          OutFile << delta << " ";
          OutFile << traveltime << " ";
          OutFile << ((directphase == true) ? '1' : '0') << " ";
          OutFile << takeoff << " ";
          OutFile << aoi << " ";
          OutFile << kk << " ";
          OutFile << ray_dist << std::endl;
        }
        OutFile.close();
        VelocityFile.close();
        return 0;
      }
      VelocityFile.close();
    }

    //---- Draw fault with multiple nodal planes and exit program.
    if (DrawStationsOnly && (DrawFaultsOnly || DrawFaultOnly)) {
      DrawFaultsStations(FaultString, StationString, FilenameOut, Size);
      return 0;
    }

    if (DrawStationsOnly) {
      PlotStations(StationString, FilenameOut, Size);
      return 0;
    }

    if (DrawFaultsOnly) {
      DrawFaults(FaultString, FilenameOut, Size);
      return 0;
    }

    //---- Draw fault with single nodal plane and exit program.
    if (DrawFaultOnly) {
      DrawFault(FaultString, FilenameOut, Size);
      return 0;
    }

    // Prepare processing structure.
    Taquart::NormType InversionNormType =
        (NormType == "L2") ? Taquart::ntL2 : Taquart::ntL1;
    int Type = 1;

    //---- Read input file and fill input data structures.
    Taquart::SMTInputData InputData;
    char id[50], phase[10], component[10], fileid[50];
    double moment = 0.0;
    double azimuth = 0.0, takeoff = 0.0, velocity = 0.0, distance = 0.0,
        density = 0.0, aoi = 0.0;

    std::ifstream InputFile;
    InputFile.open(FilenameIn.c_str());
    while (InputFile.good()) {
      InputData.Clear();
      if (VelocityModel) {
        // Reading formatted input file (velocity model format).
        double e_northing = 0.0f, e_easting = 0.0f, e_z = 0.0f;
        double s_northing = 0.0f, s_easting = 0.0f, s_z = 0.0f;
        InputFile >> fileid;
        InputFile >> N;
        InputFile >> e_northing;
        InputFile >> e_easting;
        InputFile >> e_z;
        InputFile >> density;
        for (unsigned int i = 0; i < N; i++) {
          InputFile >> id;
          InputFile >> component;
          InputFile >> phase;
          InputFile >> moment; // Should hold the area below the first P-wave velocity pulse (=moment)
          InputFile >> s_northing;
          InputFile >> s_easting;
          InputFile >> s_z;

          // Calculation of azimuth, takeoff, velocity and distance.
          double depth = fabs(e_z * 0.001);
          double elevation = s_z * 0.001;
          double epicentral_distance = 0.001
              * sqrt(
                  pow(s_northing - e_northing, 2.0)
                      + pow(s_easting - e_easting, 2.0));
          azimuth = atan2(s_easting - e_easting, s_northing - e_northing)
              * 180/ M_PI;
          double null;
          bool null2;
          int null3;

          for (unsigned int j = Velocity.size() - 1; j >= 0; j--) {
            if (depth >= Top[j]) {
              velocity = Velocity[j];
              break;
            }
          }
          CalcTravelTime1D_2(elevation, depth, epicentral_distance, Top,
              Velocity, null, takeoff, null2, aoi, null3, distance);
          // Prepare input line structure.
          Taquart::SMTInputLine il;
          il.Name = Taquart::String(id); /*!< Station name.*/
          il.Id = i + 1; /*!< Station id number.*/
          il.Component = Taquart::String(component);
          il.MarkerType = Taquart::String(phase);
          il.Start = 0.0;
          il.End = 0.0;
          il.Duration = 0.0;
          il.Displacement = moment / cos(aoi * M_PI / 180.0); // area below the first P wave pulse is divided by angle of incicence. (vertical sensor)
          il.Incidence = aoi;
          il.Azimuth = azimuth;
          il.TakeOff = takeoff;
          il.Distance = distance * 1000;
          il.Density = density;
          il.Velocity = velocity * 1000;
          il.PickActive = true;
          il.ChannelActive = true;
          InputData.Add(il);
        }
      }
      else {
        // Read formatted input file (standard foci-mt format)
        InputFile >> fileid;
        InputFile >> N;
        for (unsigned int i = 0; i < N; i++) {
          InputFile >> id;
          InputFile >> component;
          InputFile >> phase;
          InputFile >> moment; // Should hold area below the first P-wave velocity pulse
          InputFile >> azimuth;
          InputFile >> aoi;
          InputFile >> takeoff;
          InputFile >> velocity;
          InputFile >> distance;
          InputFile >> density;

          // Prepare input line structure.
          Taquart::SMTInputLine il;
          il.Name = Taquart::String(id); /*!< Station name.*/
          il.Id = i + 1; /*!< Station id number.*/
          il.Component = Taquart::String(component); //"ZZ";       /*!< Component.*/
          il.MarkerType = Taquart::String(phase); //"p*ons/p*max";      /*!< Type of the marker used.*/
          il.Start = 0.0; //tstart;;           /*!< Start time [s].*/
          il.End = 0.0; //tend;;             /*!< End time [s].*/
          il.Duration = 0.0; /*!< Duration of first P-wave velocity pulse [s].*/
          il.Displacement = moment / cos(aoi * M_PI / 180.0); /*!< Seismic moment of the first P-wave displacement pulse (only P wave, Z component) [m]. */
          il.Incidence = aoi; //incidence;       /*!< Angle of incidence [deg] (not used here) */
          il.Azimuth = azimuth; /*!< Azimuth between station and source [deg]. */
          il.TakeOff = takeoff; /*!< Takeoff angle measured from bottom [deg]. */
          il.Distance = distance; /*!< Distance between station and source [m]. */
          il.Density = density; /*!< Density in the source [km/m**3]. */
          il.Velocity = velocity; /*!< Velocity in the source [m/s]. */
          il.PickActive = true;
          il.ChannelActive = true;
          InputData.Add(il);
        }
      }

      if (!InputFile.good())
        continue;

      std::vector<Taquart::FaultSolutions> FSList;
      Taquart::FaultSolution fu, tr, dc;

      //=======================================================================
      //==== Perform regular moment tensor inversions using all stations ======
      //=======================================================================
      MTInversion(InversionNormType, QualityType, InputData, 0, 'N', FSList);

      //=======================================================================
      //==== Perform additional moment tensor inversions ======================
      //=======================================================================
      if (NoiseTest) {

        const Taquart::SMTInputData fd = InputData;
        for (unsigned int i = 0; i < AmplitudeN; i++) {
          Taquart::SMTInputData td = fd;
          Taquart::SMTInputLine InputLine;

          int sample;
          double u1, u2, z;
          for (unsigned int j = 0; j < td.Count(); j++) {
            td.Get(j, InputLine);
            sample = rand();
            u1 = (sample + 1) / (double(RAND_MAX) + 1);
            sample = rand();
            u2 = (sample + 1) / (double(RAND_MAX) + 1);
            z = sqrt(-2.0 * log(u1)) * cos(2 * M_PI * u2);
            InputLine.Displacement = InputLine.Displacement
                + z / 3.0 * InputLine.Displacement * AmpFactor;
            td.Set(j, InputLine);
          }

          // Run MT inversion for biased dataset.
          MTInversion(InversionNormType, QualityType, td, 0, 'A', FSList);
        }
      }
      else if (JacknifeTest) {
        const Taquart::SMTInputData fd = InputData;
        const unsigned int Count = InputData.Count();

        // Remove one channel, calculate the jacknife solution (option -j)
        for (unsigned int i = 0; i < Count; i++) {
          Taquart::SMTInputData td = fd;
          Taquart::SMTInputLine InputLine;
          td.Get(i, InputLine);
          int channel = InputLine.Id;
          td.Remove(i);

          // Run MT inversion for jacknife dataset
          MTInversion(InversionNormType, QualityType, td, channel, 'J', FSList);
        }
      }
      // Perform additional inversions using resampled datasets
      // Options -rr/-rp/-ra/-rt
      else if (BootstrapTest) {
        Taquart::SMTInputData BootstrapData;
        Taquart::SMTInputLine InputLine;

        for (unsigned int i = 0; i < BootstrapSamples; i++) {

          // Get original input data.
          BootstrapData = InputData;

          // Proceed through phase data for single event.
          unsigned int st_rejected = 0;
          unsigned int st_reversed = 0;
          unsigned int st_ampmod = 0;
          double v;
          for (unsigned int j = 0; j < BootstrapData.Count(); j++) {

            // Randomly modify station takeoff angle (option -rt)
            if (BootstrapTakeoffModifier > 0.0) {
              v = rand_normal(0.0, BootstrapTakeoffModifier);
              BootstrapData.Get(j, InputLine);
              InputLine.TakeOff = InputLine.TakeOff + v / 3.0;
              BootstrapData.Set(j, InputLine);
            }

            // Randomly reverse station polarity (option -rp)
            if (BootstrapPercentReverse > 0.0
                && rand() % 10000 < BootstrapPercentReverse * 10000.0) {
              BootstrapData.Get(j, InputLine);
              InputLine.Displacement = InputLine.Displacement * -1.0;
              BootstrapData.Set(j, InputLine);
              st_reversed++;
            }

            // Randomly modify station amplitude (option -ra)
            if (BootstrapAmplitudeModifier > 0.0) {
              v = rand_normal(0.0, BootstrapAmplitudeModifier);
              BootstrapData.Get(j, InputLine);
              InputLine.Displacement = InputLine.Displacement
                  + v * InputLine.Displacement / 3.0;
              BootstrapData.Set(j, InputLine);
              st_ampmod++;
            }

            // Randomly reject stations (option -rr)
            if (BootstrapPercentReject > 0.0
                && rand() % 10000 < BootstrapPercentReject * 10000.0) {
              BootstrapData.Remove(j);
              st_rejected++;
            }
          }

          int channel = i + 1;

          // Run MT inversion for resampled dataset.
          MTInversion(InversionNormType, QualityType, BootstrapData, channel,
              'B', FSList);
        }
      } // End MT inversion loop for different events

      //=======================================================================
      //==== Produce output file and graphical representation of the MT =======
      //=======================================================================

      // Beach ball properties.
      if (Projection.Pos("W") > 0)
        WulffProjection = true;
      if (Projection.Pos("S") > 0)
        WulffProjection = false;
      if (Projection.Pos("U") > 0)
        LowerHemisphere = false;
      if (Projection.Pos("L") > 0)
        LowerHemisphere = true;
      DrawStations = BallContent.Pos("S") > 0 ? true : false;
      DrawAxes = BallContent.Pos("A") > 0 ? true : false;
      DrawCross = BallContent.Pos("C") > 0 ? true : false;
      DrawDC = BallContent.Pos("D") > 0 ? true : false;

      // Text output formatted or not?
      bool Formatted = false;
      for (int i = 1; i <= DumpOrder.Length(); i++) {
        if (DumpOrder[i] >= 'a' && DumpOrder[i] <= 'z') {
          Formatted = true;
          break;
        }
      }

      //---- Export text output files if requested by the user.
      char txtb[512] = { };
      for (unsigned int j = 0; j < FSList.size(); j++) {
        Taquart::FaultSolution Solution = FSList[j].DoubleCoupleSolution;
        char Type = FSList[j].Type;
        int Channel = FSList[j].Channel;

        Taquart::String FSuffix = "dc";
        for (int i = 1; i <= SolutionTypes.Length(); i++) {
          switch (SolutionTypes[i]) {
            case 'F':
              Solution = FSList[j].FullSolution;
              FSuffix = "full";
              break;
            case 'T':
              Solution = FSList[j].TraceNullSolution;
              FSuffix = "deviatoric";
              break;
            case 'D':
              Solution = FSList[j].DoubleCoupleSolution;
              FSuffix = "dc";
              break;
          }

          // Export graphical representation
          if (OutputFileType.Pos("NONE") == 0 && j == 0) {
            Taquart::String Formats[] = { "PNG", "SVG", "PS", "PDF" };
            Taquart::TriCairo_CanvasType ctype[] = { Taquart::ctSurface,
                Taquart::ctSVG, Taquart::ctPS, Taquart::ctPDF };

            for (int q = 0; q < 4; q++) {
              if (OutputFileType.Pos(Formats[q])) {
                try {
                  Taquart::String OutName;
                  if (FilenameOut.Length() == 0) {
                    OutName = Taquart::String(fileid) + "-" + FSuffix + "."
                        + Formats[q].LowerCase();
                  }
                  else {
                    Taquart::String path;
                    Taquart::String file;
                    SplitFilename(FilenameOut, file, path);
                    if (path == file) {
                      OutName = path + "-" + Taquart::String(fileid) + "-"
                          + FSuffix + "." + Formats[q].LowerCase();
                    }
                    else {
                      OutName = path + Taquart::String("/")
                          + Taquart::String(fileid) + "-" + FSuffix + "."
                          + Formats[q].LowerCase();
                    }
                  }
                  if (ctype[q] == Taquart::ctSurface) {
                    Taquart::TriCairo_Meca Meca(Size, Size, ctype[q]);
                    GenerateBallCairo(Meca, FSList, InputData, FSuffix);
                    Meca.Save(OutName);
                  }
                  else {
                    Taquart::TriCairo_Meca Meca(Size, Size, ctype[q], OutName);
                    GenerateBallCairo(Meca, FSList, InputData, FSuffix);
                  }
                }
                catch (...) {
                  return 2;
                }
              }

            }
          }

          // Output text data if necessary.
          if (DumpOrder.Length()) {

            Taquart::String OutName;
            Taquart::String OutName2;
            if (FilenameOut.Length() == 0) {
              // No common file name, use file id instead.
              OutName = Taquart::String(fileid) + "-" + FSuffix + ".asc";
              OutName2 = Taquart::String(fileid) + "-" + FSuffix + "-u.asc";
            }
            else {
              OutName = FilenameOut + "-" + FSuffix + ".asc";
              OutName2 = FilenameOut + "-" + FSuffix + "-u.asc";
            }

            bool ExportU = DumpOrder.Pos("U") > 0 || DumpOrder.Pos("u") > 0;

            ofstream OutFile(OutName.c_str(),
                std::ofstream::out | std::ofstream::app);
            ofstream OutFile2;
            if (ExportU)
              OutFile2.open(OutName2.c_str(),
                  std::ofstream::out | std::ofstream::app);

            //bool head = false;
            //if (DumpOrder.Pos("h") || DumpOrder.Pos("H")) head = true; // TODO: Export header (not implemented yet)

            if (j == 0) {
              OutFile << fileid << FOCIMT_SEP << FSList.size() << std::endl;
              if (ExportU)
                OutFile2 << fileid << FOCIMT_SEP << FSList.size() << std::endl;
            }

            // Dump additional information when Jacknife test performed.
            if (Formatted)
              sprintf(txtb, "%c%s%5d", Type, FOCIMT_SEP2, Channel);
            else
              sprintf(txtb, "%c%s%d", Type, FOCIMT_SEP, Channel);
            OutFile << txtb;
            if (ExportU)
              OutFile2 << txtb;

            for (int i = 1; i <= DumpOrder.Length(); i++) {
              // M - moment, D - decomposition, A - axis, F - fault planes,
              // C - moment in CMT convention, * - newline

              // Export moment tensor components.
              if (DumpOrder[i] == 'M') {
                OutFile << FOCIMT_SEP << Solution.M[1][1];
                OutFile << FOCIMT_SEP << Solution.M[1][2];
                OutFile << FOCIMT_SEP << Solution.M[1][3];
                OutFile << FOCIMT_SEP << Solution.M[2][2];
                OutFile << FOCIMT_SEP << Solution.M[2][3];
                OutFile << FOCIMT_SEP << Solution.M[3][3];
              }
              else if (DumpOrder[i] == 'm') {
                sprintf(txtb,
                    "%s%13.5e%s%13.5e%s%13.5e%s%13.5e%s%13.5e%s%13.5e",
                    FOCIMT_SEP2, Solution.M[1][1], FOCIMT_SEP2,
                    Solution.M[1][2],
                    FOCIMT_SEP2, Solution.M[1][3], FOCIMT_SEP2,
                    Solution.M[2][2],
                    FOCIMT_SEP2, Solution.M[2][3], FOCIMT_SEP2,
                    Solution.M[3][3]);
                OutFile << txtb;
              }

              // Export moment tensor components in CMT convention.
              if (DumpOrder[i] == 'C') {
                OutFile << FOCIMT_SEP << Solution.M[3][3];
                OutFile << FOCIMT_SEP << Solution.M[1][1];
                OutFile << FOCIMT_SEP << Solution.M[2][2];
                OutFile << FOCIMT_SEP << Solution.M[1][3];
                OutFile << FOCIMT_SEP << -Solution.M[2][3];
                OutFile << FOCIMT_SEP << -Solution.M[1][2];
              }
              else if (DumpOrder[i] == 'c') {
                sprintf(txtb,
                    "%s%13.5e%s%13.5e%s%13.5e%s%13.5e%s%13.5e%s%13.5e",
                    FOCIMT_SEP2, Solution.M[3][3], FOCIMT_SEP2,
                    Solution.M[1][1],
                    FOCIMT_SEP2, Solution.M[2][2], FOCIMT_SEP2,
                    Solution.M[1][3],
                    FOCIMT_SEP2, -Solution.M[2][3], FOCIMT_SEP2,
                    -Solution.M[1][2]);
                OutFile << txtb;
              }

              // Export percentage of decomposed moment tensor components.
              if (DumpOrder[i] == 'D') {
                OutFile << FOCIMT_SEP << Solution.EXPL;
                OutFile << FOCIMT_SEP << Solution.CLVD;
                OutFile << FOCIMT_SEP << Solution.DBCP;
              }
              else if (DumpOrder[i] == 'd') {
                sprintf(txtb, "%s%+7.1f%s%+7.1f%s%+7.1f", FOCIMT_SEP2,
                    Solution.EXPL,
                    FOCIMT_SEP2, Solution.CLVD, FOCIMT_SEP2, Solution.DBCP);
                OutFile << txtb;
              }

              // Export eigenvalues of the moment tensor solution.
              if (DumpOrder[i] == 'G') {
                OutFile << FOCIMT_SEP << Solution.E[0];
                OutFile << FOCIMT_SEP << Solution.E[1];
                OutFile << FOCIMT_SEP << Solution.E[2];
              }
              else if (DumpOrder[i] == 'g') {
                sprintf(txtb, "%s%13.5e%s%13.5e%s%13.5e", FOCIMT_SEP2,
                    Solution.E[0],
                    FOCIMT_SEP2, Solution.E[1], FOCIMT_SEP2, Solution.E[2]);
                OutFile << txtb;
              }

              // Export percentage of decomposed moment tensor components using Vavrycuk Decomposition.
              if (DumpOrder[i] == 'Y') {
                OutFile << FOCIMT_SEP << Solution.EXPL_VAC;
                OutFile << FOCIMT_SEP << Solution.CLVD_VAC;
                OutFile << FOCIMT_SEP << Solution.DBCP_VAC;
              }
              else if (DumpOrder[i] == 'y') {
                sprintf(txtb, "%s%+7.1f%s%+7.1f%s%+7.1f", FOCIMT_SEP2,
                    Solution.EXPL_VAC,
                    FOCIMT_SEP2, Solution.CLVD_VAC, FOCIMT_SEP2,
                    Solution.DBCP_VAC);
                OutFile << txtb;
              }

              // Export axis trends and plunges.
              if (DumpOrder[i] == 'A') {
                OutFile << FOCIMT_SEP << Solution.PXTR;
                OutFile << FOCIMT_SEP << Solution.PXPL;
                OutFile << FOCIMT_SEP << Solution.TXTR;
                OutFile << FOCIMT_SEP << Solution.TXPL;
                OutFile << FOCIMT_SEP << Solution.BXTR;
                OutFile << FOCIMT_SEP << Solution.BXPL;
              }
              else if (DumpOrder[i] == 'a') {
                sprintf(txtb, "%s%5.1f%s%4.1f%s%5.1f%s%4.1f%s%5.1f%s%4.1f",
                FOCIMT_SEP2, Solution.PXTR, FOCIMT_SEP2, Solution.PXPL,
                FOCIMT_SEP2, Solution.TXTR, FOCIMT_SEP2, Solution.TXPL,
                FOCIMT_SEP2, Solution.BXTR, FOCIMT_SEP2, Solution.BXPL);
                OutFile << txtb;
              }

              // Export fault plane solutions
              if (DumpOrder[i] == 'F') {
                OutFile << FOCIMT_SEP << Solution.FIA;
                OutFile << FOCIMT_SEP << Solution.DLA;
                OutFile << FOCIMT_SEP << Solution.RAKEA;
                OutFile << FOCIMT_SEP << Solution.FIB;
                OutFile << FOCIMT_SEP << Solution.DLB;
                OutFile << FOCIMT_SEP << Solution.RAKEB;
              }
              else if (DumpOrder[i] == 'f') {
                sprintf(txtb, "%s%5.1f%s%4.1f%s%6.1f%s%5.1f%s%4.1f%s%6.1f",
                FOCIMT_SEP2, Solution.FIA, FOCIMT_SEP2, Solution.DLA,
                FOCIMT_SEP2, Solution.RAKEA, FOCIMT_SEP2, Solution.FIB,
                FOCIMT_SEP2, Solution.DLB, FOCIMT_SEP2, Solution.RAKEB);
                OutFile << txtb;
              }

              // Export seismic moments, moment magnitude and moment error
              if (DumpOrder[i] == 'W') {
                OutFile << FOCIMT_SEP << Solution.M0;
                OutFile << FOCIMT_SEP << Solution.MT;
                OutFile << FOCIMT_SEP << Solution.ERR;
                OutFile << FOCIMT_SEP << Solution.MAGN;
              }
              if (DumpOrder[i] == 'w') {
                sprintf(txtb, "%s%11.3e%s%11.3e%s%11.3e%s%6.2f", FOCIMT_SEP2,
                    Solution.M0,
                    FOCIMT_SEP2, Solution.MT, FOCIMT_SEP2, Solution.ERR,
                    FOCIMT_SEP2, Solution.MAGN);
                OutFile << txtb;
              }

              // Export quality factor.
              if (DumpOrder[i] == 'Q') {
                OutFile << FOCIMT_SEP << Solution.QI;
              }
              if (DumpOrder[i] == 'q') {
                sprintf(txtb, "%s%5.1f", FOCIMT_SEP2, Solution.QI);
                OutFile << txtb;
              }

              // Export solution type.
              if (DumpOrder[i] == 'T') {
                OutFile << FOCIMT_SEP << Solution.Type.c_str();
              }
              if (DumpOrder[i] == 't') {
                sprintf(txtb, "%s%s", FOCIMT_SEP2, Solution.Type.c_str());
                OutFile << txtb;
              }

              // Export theoretical displacements.
              if (DumpOrder[i] == 'U' && ExportU) {
                OutFile2 << FOCIMT_SEP << Solution.U_n << std::endl;
                for (int r = 0; r < Solution.U_n; r++) {
                  OutFile2 << Solution.Station[r].c_str() << FOCIMT_SEP
                      << Solution.U_measured[r] << FOCIMT_SEP
                      << Solution.U_th[r] << std::endl;
                }
              }
              else if (DumpOrder[i] == 'u' && ExportU) {
                OutFile2 << FOCIMT_SEP2 << Solution.U_n << std::endl;
                for (int r = 0; r < Solution.U_n; r++) {
                  sprintf(txtb, "%5s%s%13.5e%s%13.5e",
                      Solution.Station[r].c_str(),
                      FOCIMT_SEP2, Solution.U_measured[r], FOCIMT_SEP2,
                      Solution.U_th[r]);
                  OutFile2 << txtb << std::endl;
                }
              }

              // Export std error of displacement fit.
              if (DumpOrder[i] == 'E') {
                OutFile << FOCIMT_SEP << Solution.UERR;
              }
              else if (DumpOrder[i] == 'e') {
                sprintf(txtb, "%s%11.3e", FOCIMT_SEP2, Solution.UERR);
                OutFile << txtb;
              }

              // Export diagonal elements of covariance matrix.
              if (DumpOrder[i] == 'V') {
                for (int q = 1; q <= 6; q++) {
                  OutFile << FOCIMT_SEP << Solution.Covariance[q][q];
                }
              }
              else if (DumpOrder[i] == 'v') {
                sprintf(txtb,
                    "%s%11.3e%s%11.3e%s%11.3e%s%11.3e%s%11.3e%s%11.3e",
                    FOCIMT_SEP2, Solution.Covariance[1][1], FOCIMT_SEP2,
                    Solution.Covariance[2][2], FOCIMT_SEP2,
                    Solution.Covariance[3][3], FOCIMT_SEP2,
                    Solution.Covariance[4][4], FOCIMT_SEP2,
                    Solution.Covariance[5][5], FOCIMT_SEP2,
                    Solution.Covariance[6][6]);
                OutFile << txtb;
              }

              if (DumpOrder[i] == '*') {
                OutFile << FOCIMT_NEWLINE;
              }
            }

            OutFile << FOCIMT_NEWLINE;
            OutFile.close();

          } // Loop for all solution types.
        } // Loof for all events
      }
    }
    //InputFile.close();
    return 0;
  }
  catch (...) {
    return 1; // Some undefined error occurred, error code 1.
  }
}

