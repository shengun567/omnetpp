//==========================================================================
//  TKAPP.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __TKAPP_H
#define __TKAPP_H

#include <tk.h>

#include "omnetapp.h"

#ifdef SUNSPARC
#define TKENV_EXTRASTACK   24576
#else
#define TKENV_EXTRASTACK   16384
#endif

class Speedometer;
class TInspector;

//=========================================================================
// TOmnetTkApp: Tcl/Tk-based user interface.

class TOmnetTkApp : public TOmnetApp
{
   public:
      enum eState {
          SIM_NONET = 0,
          SIM_NEW = 1,
          SIM_RUNNING = 2,
          SIM_READY = 3,
          SIM_TERMINATED = 4,
          SIM_ERROR = 5,
          SIM_FINISHCALLED = 6
      };

      //
      // state transitions:
      //    SIM_NONET -> SIM_NEW -> (SIM_RUNNING <-> SIM_READY) -> SIM_TERMINATED -> SIM_FINISHCALLED -> SIM_NONET
      //                                               `-> SIM_ERROR

   public:
      int  opt_default_run;        // automatically set up this run at startup
      bool opt_bkpts_enabled;      // stop at breakpoints (can be improved...)
      bool opt_print_banners;      // print event banners
      bool opt_use_mainwindow;     // dump modules' ev << ... stuff into main window
      bool opt_animation_enabled;  // msg animation
      bool opt_animation_msgnames; // msg animation: display message name or not
      bool opt_animation_msgcolors;// msg animation: display msg kind as color code or not
      double opt_animation_speed;  // msg animation speed: 0=slow 1=norm 2=fast
      long opt_stepdelay;          // Delay between steps in 100th seconds
      int  opt_updatefreq_fast;    // FastRun updates display every N events
      int  opt_updatefreq_express; // RunExpress updates display every N events
      unsigned opt_extrastack;     // per-module extra stack

   protected:
      Tcl_Interp *interp;      // TCL interpreter

      opp_string tkenv_dir;    // directory of Tkenv's *.tcl files
      opp_string bitmap_dir;   // directory of icon files

      eState simstate;         // state of the simulation run
      int   run_nr;            // number of current simulation run
      bool  animation_ok;      // while execution, do message animation or not
      bool  bkpt_hit;          // flag to signal that a breakpoint was hit and sim. must be stopped
      bool  stop_simulation;   // flag to signal that simulation must be stopped (STOP button pressed in the UI)

      cHead inspectors;        // list of inspector objects

   public:
      TOmnetTkApp(ArgList *args, cIniFile *inifile);
      ~TOmnetTkApp();

      // redefined virtual functions from TOmnetApp
      virtual void setup();
      virtual int run();
      virtual void shutdown();

      virtual void objectDeleted(cObject *object); // notify environment
      virtual void messageSent(cMessage *msg);
      virtual void messageDelivered(cMessage *msg);
      virtual void breakpointHit(const char *lbl, cSimpleModule *mod);

      virtual void putmsg(const char *s);
      virtual void puts(const char *s);
      virtual bool gets(const char *promptstr, char *buf, int len=255);
      virtual int  askYesNo(const char *question);

      virtual void readOptions();
      virtual void readPerRunOptions(int run_nr);

      // if using Tkenv, modules should have ~16K extra stack
      virtual unsigned extraStackForEnvir();

      // New functions:
      void newNetwork( const char *networkname );
      void newRun( int run_no );
      void createSnapshot(const char *label);

      void rebuildSim();
      void doOneStep();
      void runSimulation(simtime_t until_time, long until_event,
                         bool slowexec, bool fast,
                         cSimpleModule *stopinmod=NULL);
      void runSimulationExpress( simtime_t until_time, long until_event );
      void startAll();
      void finishSimulation(); // wrapper around simulation.callFinish() and simulation.endRun()

      void inspectorByName();
      void newMsgWindow();
      void newFileWindow();

      bool isBreakpointActive(const char *label, cSimpleModule *mod);
      void stopAtBreakpoint(const char *label, cSimpleModule *mod);

      void updateInspectors();
      TInspector *inspect(cObject *obj, int type, const char *geometry, void *dat);
      TInspector *findInspector(cObject *obj, int type);

      int getSimulationState() {return simstate;}
      void setStopSimulationFlag() {stop_simulation = true;}
      Tcl_Interp *getInterp() {return interp;}

      // small functions:
      void updateNetworkRunDisplay();
      void updateSimtimeDisplay();
      void updateNextModuleDisplay();
      void clearNextModuleDisplay();
      void updatePerformanceDisplay(Speedometer& speedometer);
      void clearPerformanceDisplay();

      void printEventBanner(cSimpleModule *mod);

      const char *getIniFileName()       {return opt_inifile_name;}
      const char *getOutVectorFileName() {return outvectmgr->fileName();}
      const char *getOutScalarFileName() {return outscalarmgr->fileName();}
      const char *getSnapshotFileName()  {return snapshotmgr->fileName();}
};

#endif
