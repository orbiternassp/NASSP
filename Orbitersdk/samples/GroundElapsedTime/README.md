# GroundElapsedTime MFD (for NASSP / Orbiter)

This is a small "MFD" (multi-function display) addon for the [Orbiter space
flight simulator](http://orbit.medphys.ucl.ac.uk/), built specifically to
work alongside [NASSP](https://github.com/orbiternassp/NASSP) (the Project
Apollo addon for Orbiter).

It shows one thing: **Ground Elapsed Time (GET)**, also called Mission
Elapsed Time (MET) - basically a stopwatch that starts at zero when an
Apollo mission lifts off. It also writes that same value out to a small
text file once a second, so a separate program (not part of Orbiter at all)
can read it too - for example, to play back a mission transcript in sync
with what's happening in the simulator.

If you've never built a Visual Studio project before, don't worry - every
step below is spelled out.

## What makes this different from just using NASSP's own MFD?

NASSP already ships an MFD (called "Project Apollo") that can show GET,
among many other things. This addon is a much simpler, standalone MFD that
*only* shows GET, using the exact same logic and the exact same NASSP
vessel classes (`Saturn`, `LEM`, `SIVB`, `Crawler`, `MCC`) that NASSP's own
MFD uses internally - so the number is always identical to what NASSP
itself considers the mission time to be. Because of that, this addon must
be built the same way NASSP itself is built (32-bit/"Win32", not 64-bit),
and it only works when NASSP is installed - see "Requirements" below.

## What you get

1. **An MFD mode called "Ground Elapsed Time"** with a button labeled `GET`.
   Select it on any Saturn, LEM, S-IVB, or Crawler vessel and it shows the
   mission time as `H:MM:SS` (a leading `-` means the countdown is still
   before liftoff).
2. **A text file, `GroundElapsedTime.txt`**, written directly in your main
   Orbiter folder (right next to `Orbiter.exe`), overwritten about once
   every second of simulation time. Its contents are just four numbers,
   zero-padded, separated by spaces, with no labels:

   ```
   00 03 14 07
   ```

   In order, that's **days, hours, minutes, seconds** of Ground Elapsed
   Time (so the example above is 3 hours, 14 minutes, 7 seconds - 0 days -
   since liftoff). This format matches how NASA's own mission transcripts
   usually break down GET (days/hours/minutes/seconds), which makes it
   easy for another program to compare its timestamps against this file.
   Before liftoff (or if nothing supported is currently focused), the file
   just contains `00 00 00 00`.

## Requirements

- Microsoft Visual Studio 2017 or newer (NASSP's own build instructions
  ask for the same thing).
- A working copy of Orbiter with NASSP already installed and building
  successfully. This addon's code directly includes NASSP's own C++
  header files (`saturn.h`, `LEM.h`, etc.) from
  `Orbitersdk/samples/ProjectApollo/`, so it must live inside the same
  NASSP source tree, and Orbiter's SDK (`Orbitersdk/include`,
  `Orbitersdk/lib`) must already be present the same way it is for every
  other NASSP project.
- This must be built as **Win32 (32-bit)**, in the **Release** (or Debug)
  configuration - NOT x64. All of NASSP (Saturn, LEM, MCC, SIVB, and the
  existing Project Apollo MFD) is 32-bit only today, so this addon has to
  match that exactly to be able to read their internal mission-time values
  safely. A 64-bit build would not be compatible with a 32-bit NASSP
  installation at all.

## How to build it

1. Open `Orbitersdk/samples/ProjectApollo/ProjectApollo2017.sln` in Visual
   Studio (this is the same solution file NASSP itself uses - this addon
   has been added to it as one more project, called "GroundElapsedTime").
2. At the top of Visual Studio, set the configuration dropdown to
   **Release** and the platform dropdown to **Win32** (do not pick x64).
3. In the Solution Explorer panel, right-click the **GroundElapsedTime**
   project and choose **Build** (or just build the whole solution with
   *Build > Build Solution* if you want everything, including NASSP
   itself, rebuilt).
4. If it builds successfully, you'll get
   `GroundElapsedTime.dll` inside your Orbiter installation's
   `Modules\Plugin\` folder automatically (the project is already set up
   to put it there directly - no manual copying needed if your NASSP
   source tree lives inside your actual Orbiter folder, which is the usual
   NASSP setup).

## How to install it (if you got the DLL some other way)

If you (or someone else) already built `GroundElapsedTime.dll` and just
want to install it:

1. Copy `GroundElapsedTime.dll` into your Orbiter installation's
   `Modules\Plugin\` folder (the same folder where NASSP's own
   `ProjectApolloMFD.dll` lives).
2. Start Orbiter, load any NASSP scenario with a Saturn/LEM/S-IVB/Crawler.
3. Open any MFD (e.g. left MFD), press the **"Sel"** button, and keep
   pressing it (or use the MFD mode selection key) until **"Ground Elapsed
   Time"** appears.
4. To see the exported file, just open `GroundElapsedTime.txt` (in your
   main Orbiter folder, next to `Orbiter.exe`) in Notepad while the
   simulation is running - the numbers should update roughly once a second.

## Files in this folder

- `src/GroundElapsedTimeMFD.h` / `.cpp` - the on-screen MFD itself.
- `src/GroundElapsedTimeCommon.h` / `.cpp` - shared logic for "given a
  vessel, what is its current Ground Elapsed Time?", used by both the MFD
  and the file-export feature so they can never disagree with each other.
- `src/GroundElapsedTimeExport.h` / `.cpp` - the once-a-second file-export
  feature.
- `Build/VC2017/GroundElapsedTime.vcxproj` - the Visual Studio project file.

## Notes / limitations

- This first version only exports to a file. A network-based option (so
  another program could read GET without touching a file on disk) was
  considered but intentionally left out for now, to keep this first
  version as simple and easy to debug as possible.
- Every part of the GET calculation itself is copied from NASSP's own
  proven code (`ProjectApolloMFD.cpp`'s "Draw mission time" section and
  `ProjectApolloChecklistMFD.cpp`'s `DisplayMissionElapsedTime()`), so it
  should behave identically to what NASSP's existing "Project Apollo" MFD
  already shows.
