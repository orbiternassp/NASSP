#pragma once

// Needed in P27 update
#include <stdio.h>
// Winsock2
#include "winsock2.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace GroundStation
{
	/// <summary> 
	/// Summary for UplinkForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	__gc class UplinkForm : public System::Windows::Forms::Form
	{
	public: 
		UplinkForm(void)
		{
			InitializeComponent();
			this->add_Closing(new CancelEventHandler(this, &GroundStation::UplinkForm::UplinkForm_Closing)); // Setup closing event
			cmc_uplink_state = 0;
		}
        
	protected: 
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}

	private: System::Void UplinkForm_Closing(Object * sender, System::ComponentModel::CancelEventArgs* e){			
			e->Cancel = true; // don't close			
			this->Hide();     // Hide instead.
		}	

	private: System::Void UplinkForm::P27IndexChanged(Object * sender, System::EventArgs * e){
		char tmp[256];
		int index=0;
		// Address to ANSI string
		sprintf(tmp,"%s",System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(this->textBox5->Text));
		if(strlen(tmp) < 1 || strlen(tmp) > 4){
			index=0; // OOB
		}else{
			index = atoi(tmp);
		}
		if(index > 20){ index = 0; } // OOB
		// Now enable accordingly
		if(index >= 2){ this->textBox6->Enabled = true; }else{ this->textBox6->Enabled = false; }		
		if(index >= 3){ this->textBox7->Enabled = true; }else{ this->textBox7->Enabled = false; }		
		if(index >= 4){ this->textBox9->Enabled = true; }else{ this->textBox9->Enabled = false; }		
		if(index >= 5){ this->textBox8->Enabled = true; }else{ this->textBox8->Enabled = false; }		
		if(index >= 6){ this->textBox11->Enabled = true; }else{ this->textBox11->Enabled = false; }		
		if(index >= 7){ this->textBox10->Enabled = true; }else{ this->textBox10->Enabled = false; }		
		if(index >= 8){ this->textBox13->Enabled = true; }else{ this->textBox13->Enabled = false; }		
		if(index >= 9){ this->textBox12->Enabled = true; }else{ this->textBox12->Enabled = false; }		
		if(index >= 10){ this->textBox15->Enabled = true; }else{ this->textBox15->Enabled = false; }		
		if(index >= 11){ this->textBox14->Enabled = true; }else{ this->textBox14->Enabled = false; }		
		if(index >= 12){ this->textBox25->Enabled = true; }else{ this->textBox25->Enabled = false; }		
		if(index >= 13){ this->textBox24->Enabled = true; }else{ this->textBox24->Enabled = false; }		
		if(index >= 14){ this->textBox23->Enabled = true; }else{ this->textBox23->Enabled = false; }		
		if(index >= 15){ this->textBox22->Enabled = true; }else{ this->textBox22->Enabled = false; }		
		if(index >= 16){ this->textBox21->Enabled = true; }else{ this->textBox21->Enabled = false; }		
		if(index >= 17){ this->textBox20->Enabled = true; }else{ this->textBox20->Enabled = false; }		
		if(index >= 18){ this->textBox19->Enabled = true; }else{ this->textBox19->Enabled = false; }		
		if(index >= 19){ this->textBox18->Enabled = true; }else{ this->textBox18->Enabled = false; }
		if(index >= 20){ this->textBox17->Enabled = true; }else{ this->textBox17->Enabled = false; }
	}
	
	private: System::Void P27ModeChanged(Object * sender, System::EventArgs* e){
			switch(this->comboBox1->SelectedIndex){
				case 1:  // V70 LIFTOFF TIME INCREMENT
				case 12: // V73 OCTAL CLOCK INCREMENT
					// Set labels
					this->label6->Text  = "LEFT HW";  this->label7->Text  = "RIGHT HW";
					this->label10->Text = "UNUSED";  this->label9->Text  = "UNUSED";
					this->label12->Text = "UNUSED";  this->label11->Text = "UNUSED";
					this->label14->Text = "UNUSED";  this->label13->Text = "UNUSED";
					this->label16->Text = "UNUSED";  this->label15->Text = "UNUSED";
					this->label26->Text = "UNUSED";  this->label25->Text = "UNUSED";
					this->label24->Text = "UNUSED";  this->label23->Text = "UNUSED";
					this->label22->Text = "UNUSED";  this->label21->Text = "UNUSED";
					this->label20->Text = "UNUSED";  this->label19->Text = "UNUSED";
					this->label18->Text = "UNUSED"; 
					// Disable INDEX and set to default
					this->textBox5->Text = "XX";
					this->textBox5->Enabled = false;
					// Default number fields
					this->textBox6->Text = "";  this->textBox6->Enabled = true;
					this->textBox7->Text = "";  this->textBox7->Enabled = true;
					this->textBox8->Text = "";  this->textBox8->Enabled = false;
					this->textBox9->Text = "";  this->textBox9->Enabled = false;
					this->textBox10->Text = ""; this->textBox10->Enabled = false;
					this->textBox11->Text = ""; this->textBox11->Enabled = false;
					this->textBox12->Text = ""; this->textBox12->Enabled = false;
					this->textBox13->Text = ""; this->textBox13->Enabled = false;
					this->textBox14->Text = ""; this->textBox14->Enabled = false;
					this->textBox15->Text = ""; this->textBox15->Enabled = false;
					this->textBox17->Text = ""; this->textBox17->Enabled = false;
					this->textBox18->Text = ""; this->textBox18->Enabled = false;
					this->textBox19->Text = ""; this->textBox19->Enabled = false;
					this->textBox20->Text = ""; this->textBox20->Enabled = false;
					this->textBox21->Text = ""; this->textBox21->Enabled = false;
					this->textBox22->Text = ""; this->textBox22->Enabled = false;
					this->textBox23->Text = ""; this->textBox23->Enabled = false;
					this->textBox24->Text = ""; this->textBox24->Enabled = false;
					this->textBox25->Text = ""; this->textBox25->Enabled = false;
					// Enable TRANSMIT button
					this->button64->Enabled = true;
					break;

				case 2: // V71 CONTIGUOUS ERASABLE
					// Set labels
					this->label6->Text  = "ADDRESS"; this->label7->Text  = "DATA 1";
					this->label10->Text = "DATA 2";  this->label9->Text  = "DATA 3";
					this->label12->Text = "DATA 4";  this->label11->Text = "DATA 5";
					this->label14->Text = "DATA 6";  this->label13->Text = "DATA 7";
					this->label16->Text = "DATA 8";  this->label15->Text = "DATA 9";
					this->label26->Text = "DATA 10"; this->label25->Text = "DATA 11";
					this->label24->Text = "DATA 12"; this->label23->Text = "DATA 13";
					this->label22->Text = "DATA 14"; this->label21->Text = "DATA 15";
					this->label20->Text = "DATA 16"; this->label19->Text = "DATA 17";
					this->label18->Text = "DATA 18"; 
					// Default number fields
					this->textBox6->Text = "";  this->textBox6->Enabled = false;
					this->textBox7->Text = "";  this->textBox7->Enabled = false;
					this->textBox8->Text = "";  this->textBox8->Enabled = false;
					this->textBox9->Text = "";  this->textBox9->Enabled = false;
					this->textBox10->Text = ""; this->textBox10->Enabled = false;
					this->textBox11->Text = ""; this->textBox11->Enabled = false;
					this->textBox12->Text = ""; this->textBox12->Enabled = false;
					this->textBox13->Text = ""; this->textBox13->Enabled = false;
					this->textBox14->Text = ""; this->textBox14->Enabled = false;
					this->textBox15->Text = ""; this->textBox15->Enabled = false;
					this->textBox17->Text = ""; this->textBox17->Enabled = false;
					this->textBox18->Text = ""; this->textBox18->Enabled = false;
					this->textBox19->Text = ""; this->textBox19->Enabled = false;
					this->textBox20->Text = ""; this->textBox20->Enabled = false;
					this->textBox21->Text = ""; this->textBox21->Enabled = false;
					this->textBox22->Text = ""; this->textBox22->Enabled = false;
					this->textBox23->Text = ""; this->textBox23->Enabled = false;
					this->textBox24->Text = ""; this->textBox24->Enabled = false;
					this->textBox25->Text = ""; this->textBox25->Enabled = false;
					// Enable INDEX and set to default
					this->textBox5->Text = "3";
					this->textBox5->Enabled = true;
					this->P27IndexChanged(NULL,NULL);
					// Enable TRANSMIT button
					this->button64->Enabled = true;
					break;

				case 3: // V71 STATE VECTOR
					// Set labels
					this->label6->Text  = "ADDRESS";   this->label7->Text  = "IDENT";
					this->label10->Text = "Rx LF HW";  this->label9->Text  = "Rx RT HW";
					this->label12->Text = "Ry LF HW";  this->label11->Text = "Ry RT HW";
					this->label14->Text = "Rz LF HW";  this->label13->Text = "Rz RT HW";
					this->label16->Text = "Vx LF HW";  this->label15->Text = "Vx RT HW";
					this->label26->Text = "Vy LF HW";  this->label25->Text = "Vy RT HW";
					this->label24->Text = "Vz LF HW";  this->label23->Text = "Vz RT HW";
					this->label22->Text = "Tsv LF HW"; this->label21->Text = "Tsv RT HW";
					this->label20->Text = "UNUSED";    this->label19->Text = "UNUSED";
					this->label18->Text = "UNUSED"; 
					// Default number fields
					this->textBox6->Text = "1501"; this->textBox6->Enabled = false;
					this->textBox7->Text = "";  this->textBox7->Enabled = false;
					this->textBox8->Text = "";  this->textBox8->Enabled = false;
					this->textBox9->Text = "";  this->textBox9->Enabled = false;
					this->textBox10->Text = ""; this->textBox10->Enabled = false;
					this->textBox11->Text = ""; this->textBox11->Enabled = false;
					this->textBox12->Text = ""; this->textBox12->Enabled = false;
					this->textBox13->Text = ""; this->textBox13->Enabled = false;
					this->textBox14->Text = ""; this->textBox14->Enabled = false;
					this->textBox15->Text = ""; this->textBox15->Enabled = false;
					this->textBox17->Text = ""; this->textBox17->Enabled = false;
					this->textBox18->Text = ""; this->textBox18->Enabled = false;
					this->textBox19->Text = ""; this->textBox19->Enabled = false;
					this->textBox20->Text = ""; this->textBox20->Enabled = false;
					this->textBox21->Text = ""; this->textBox21->Enabled = false;
					this->textBox22->Text = ""; this->textBox22->Enabled = false;
					this->textBox23->Text = ""; this->textBox23->Enabled = false;
					this->textBox24->Text = ""; this->textBox24->Enabled = false;
					this->textBox25->Text = ""; this->textBox25->Enabled = false;
					// Enable INDEX and set to default
					this->textBox5->Text = "17";
					this->textBox5->Enabled = true;
					this->P27IndexChanged(NULL,NULL);
					// Enable TRANSMIT button
					this->button64->Enabled = true;
					break;

				case 4: // V71 DESIRED REFSMMAT
					// Set labels
					this->label6->Text  = "ADDRESS";   this->label7->Text  = "DATA 1-LF";
					this->label10->Text = "DATA 1-RT"; this->label9->Text  = "DATA 2-LF";
					this->label12->Text = "DATA 2-RT"; this->label11->Text = "DATA 3-LF";
					this->label14->Text = "DATA 3-RT"; this->label13->Text = "DATA 4-LF";
					this->label16->Text = "DATA 4-RT"; this->label15->Text = "DATA 5-LF";
					this->label26->Text = "DATA 5-RT"; this->label25->Text = "DATA 6-LF";
					this->label24->Text = "DATA 6-RT"; this->label23->Text = "DATA 7-LF";
					this->label22->Text = "DATA 7-RT"; this->label21->Text = "DATA 8-LF";
					this->label20->Text = "DATA 8-RT"; this->label19->Text = "DATA 9-LF";
					this->label18->Text = "DATA 9-RT"; 
					// Default number fields
					this->textBox6->Text = "306"; this->textBox6->Enabled = false;
					this->textBox7->Text = "";  this->textBox7->Enabled = false;
					this->textBox8->Text = "";  this->textBox8->Enabled = false;
					this->textBox9->Text = "";  this->textBox9->Enabled = false;
					this->textBox10->Text = ""; this->textBox10->Enabled = false;
					this->textBox11->Text = ""; this->textBox11->Enabled = false;
					this->textBox12->Text = ""; this->textBox12->Enabled = false;
					this->textBox13->Text = ""; this->textBox13->Enabled = false;
					this->textBox14->Text = ""; this->textBox14->Enabled = false;
					this->textBox15->Text = ""; this->textBox15->Enabled = false;
					this->textBox17->Text = ""; this->textBox17->Enabled = false;
					this->textBox18->Text = ""; this->textBox18->Enabled = false;
					this->textBox19->Text = ""; this->textBox19->Enabled = false;
					this->textBox20->Text = ""; this->textBox20->Enabled = false;
					this->textBox21->Text = ""; this->textBox21->Enabled = false;
					this->textBox22->Text = ""; this->textBox22->Enabled = false;
					this->textBox23->Text = ""; this->textBox23->Enabled = false;
					this->textBox24->Text = ""; this->textBox24->Enabled = false;
					this->textBox25->Text = ""; this->textBox25->Enabled = false;
					// Enable INDEX and set to default
					this->textBox5->Text = "20";
					this->textBox5->Enabled = true;
					this->P27IndexChanged(NULL,NULL);
					// Enable TRANSMIT button
					this->button64->Enabled = true;
					break;

				case 5: // V71 CMC REFSMMAT
					// Set labels
					this->label6->Text  = "ADDRESS";   this->label7->Text  = "DATA 1-LF";
					this->label10->Text = "DATA 1-RT"; this->label9->Text  = "DATA 2-LF";
					this->label12->Text = "DATA 2-RT"; this->label11->Text = "DATA 3-LF";
					this->label14->Text = "DATA 3-RT"; this->label13->Text = "DATA 4-LF";
					this->label16->Text = "DATA 4-RT"; this->label15->Text = "DATA 5-LF";
					this->label26->Text = "DATA 5-RT"; this->label25->Text = "DATA 6-LF";
					this->label24->Text = "DATA 6-RT"; this->label23->Text = "DATA 7-LF";
					this->label22->Text = "DATA 7-RT"; this->label21->Text = "DATA 8-LF";
					this->label20->Text = "DATA 8-RT"; this->label19->Text = "DATA 9-LF";
					this->label18->Text = "DATA 9-RT"; 
					// Default number fields
					this->textBox6->Text = "1735"; this->textBox6->Enabled = false;
					this->textBox7->Text = "";  this->textBox7->Enabled = false;
					this->textBox8->Text = "";  this->textBox8->Enabled = false;
					this->textBox9->Text = "";  this->textBox9->Enabled = false;
					this->textBox10->Text = ""; this->textBox10->Enabled = false;
					this->textBox11->Text = ""; this->textBox11->Enabled = false;
					this->textBox12->Text = ""; this->textBox12->Enabled = false;
					this->textBox13->Text = ""; this->textBox13->Enabled = false;
					this->textBox14->Text = ""; this->textBox14->Enabled = false;
					this->textBox15->Text = ""; this->textBox15->Enabled = false;
					this->textBox17->Text = ""; this->textBox17->Enabled = false;
					this->textBox18->Text = ""; this->textBox18->Enabled = false;
					this->textBox19->Text = ""; this->textBox19->Enabled = false;
					this->textBox20->Text = ""; this->textBox20->Enabled = false;
					this->textBox21->Text = ""; this->textBox21->Enabled = false;
					this->textBox22->Text = ""; this->textBox22->Enabled = false;
					this->textBox23->Text = ""; this->textBox23->Enabled = false;
					this->textBox24->Text = ""; this->textBox24->Enabled = false;
					this->textBox25->Text = ""; this->textBox25->Enabled = false;
					// Enable INDEX and set to default
					this->textBox5->Text = "20";
					this->textBox5->Enabled = true;
					this->P27IndexChanged(NULL,NULL);
					// Enable TRANSMIT button
					this->button64->Enabled = true;
					break;

				case 6: // V71 EXTERNAL DV
					// Set labels
					this->label6->Text  = "ADDRESS"; this->label7->Text  = "DVx LF";
					this->label10->Text = "DVx RT";  this->label9->Text  = "DVy LF";
					this->label12->Text = "DVy RT";  this->label11->Text = "DVz LF";
					this->label14->Text = "DVz RT";  this->label13->Text = "TIG LF";
					this->label16->Text = "TIG RT";  this->label15->Text = "UNUSED";
					this->label26->Text = "UNUSED";  this->label25->Text = "UNUSED";
					this->label24->Text = "UNUSED";  this->label23->Text = "UNUSED";
					this->label22->Text = "UNUSED";  this->label21->Text = "UNUSED";
					this->label20->Text = "UNUSED";  this->label19->Text = "UNUSED";
					this->label18->Text = "UNUSED"; 
					// Default number fields
					this->textBox6->Text = "3404"; this->textBox6->Enabled = false;
					this->textBox7->Text = "";  this->textBox7->Enabled = false;
					this->textBox8->Text = "";  this->textBox8->Enabled = false;
					this->textBox9->Text = "";  this->textBox9->Enabled = false;
					this->textBox10->Text = ""; this->textBox10->Enabled = false;
					this->textBox11->Text = ""; this->textBox11->Enabled = false;
					this->textBox12->Text = ""; this->textBox12->Enabled = false;
					this->textBox13->Text = ""; this->textBox13->Enabled = false;
					this->textBox14->Text = ""; this->textBox14->Enabled = false;
					this->textBox15->Text = ""; this->textBox15->Enabled = false;
					this->textBox17->Text = ""; this->textBox17->Enabled = false;
					this->textBox18->Text = ""; this->textBox18->Enabled = false;
					this->textBox19->Text = ""; this->textBox19->Enabled = false;
					this->textBox20->Text = ""; this->textBox20->Enabled = false;
					this->textBox21->Text = ""; this->textBox21->Enabled = false;
					this->textBox22->Text = ""; this->textBox22->Enabled = false;
					this->textBox23->Text = ""; this->textBox23->Enabled = false;
					this->textBox24->Text = ""; this->textBox24->Enabled = false;
					this->textBox25->Text = ""; this->textBox25->Enabled = false;
					// Enable INDEX and set to default
					this->textBox5->Text = "10";
					this->textBox5->Enabled = true;
					this->P27IndexChanged(NULL,NULL);
					// Enable TRANSMIT button
					this->button64->Enabled = true;
					break;

				case 7: // V71 ENTRY DATA + EXTERNAL DV
					// Set labels
					this->label6->Text  = "ADDRESS"; this->label7->Text  = "LAT LF";
					this->label10->Text = "LAT RT";  this->label9->Text  = "LON LF";
					this->label12->Text = "LON RT";  this->label11->Text = "DVx LF";
					this->label14->Text = "DVx RT";  this->label13->Text = "DVy LF";
					this->label16->Text = "DVy RT";  this->label15->Text = "DVz LF";
					this->label26->Text = "DVz RT";  this->label25->Text = "TIG LF";
					this->label24->Text = "TIG RT";  this->label23->Text = "UNUSED";
					this->label22->Text = "UNUSED";  this->label21->Text = "UNUSED";
					this->label20->Text = "UNUSED";  this->label19->Text = "UNUSED";
					this->label18->Text = "UNUSED"; 
					// Default number fields
					this->textBox6->Text = "3400"; this->textBox6->Enabled = false;
					this->textBox7->Text = "";  this->textBox7->Enabled = false;
					this->textBox8->Text = "";  this->textBox8->Enabled = false;
					this->textBox9->Text = "";  this->textBox9->Enabled = false;
					this->textBox10->Text = ""; this->textBox10->Enabled = false;
					this->textBox11->Text = ""; this->textBox11->Enabled = false;
					this->textBox12->Text = ""; this->textBox12->Enabled = false;
					this->textBox13->Text = ""; this->textBox13->Enabled = false;
					this->textBox14->Text = ""; this->textBox14->Enabled = false;
					this->textBox15->Text = ""; this->textBox15->Enabled = false;
					this->textBox17->Text = ""; this->textBox17->Enabled = false;
					this->textBox18->Text = ""; this->textBox18->Enabled = false;
					this->textBox19->Text = ""; this->textBox19->Enabled = false;
					this->textBox20->Text = ""; this->textBox20->Enabled = false;
					this->textBox21->Text = ""; this->textBox21->Enabled = false;
					this->textBox22->Text = ""; this->textBox22->Enabled = false;
					this->textBox23->Text = ""; this->textBox23->Enabled = false;
					this->textBox24->Text = ""; this->textBox24->Enabled = false;
					this->textBox25->Text = ""; this->textBox25->Enabled = false;
					// Enable INDEX and set to default
					this->textBox5->Text = "14";
					this->textBox5->Enabled = true;
					this->P27IndexChanged(NULL,NULL);
					// Enable TRANSMIT button
					this->button64->Enabled = true;
					break;

				case 8: // V71 ENTRY DATA
					// Set labels
					this->label6->Text  = "ADDRESS"; this->label7->Text  = "LAT LF";
					this->label10->Text = "LAT RT";  this->label9->Text  = "LON LF";
					this->label12->Text = "LON RT";  this->label11->Text = "UNUSED";
					this->label14->Text = "UNUSED";  this->label13->Text = "UNUSED";
					this->label16->Text = "UNUSED";  this->label15->Text = "UNUSED";
					this->label26->Text = "UNUSED";  this->label25->Text = "UNUSED";
					this->label24->Text = "UNUSED";  this->label23->Text = "UNUSED";
					this->label22->Text = "UNUSED";  this->label21->Text = "UNUSED";
					this->label20->Text = "UNUSED";  this->label19->Text = "UNUSED";
					this->label18->Text = "UNUSED"; 
					// Default number fields
					this->textBox6->Text = "3400"; this->textBox6->Enabled = false;
					this->textBox7->Text = "";  this->textBox7->Enabled = false;
					this->textBox8->Text = "";  this->textBox8->Enabled = false;
					this->textBox9->Text = "";  this->textBox9->Enabled = false;
					this->textBox10->Text = ""; this->textBox10->Enabled = false;
					this->textBox11->Text = ""; this->textBox11->Enabled = false;
					this->textBox12->Text = ""; this->textBox12->Enabled = false;
					this->textBox13->Text = ""; this->textBox13->Enabled = false;
					this->textBox14->Text = ""; this->textBox14->Enabled = false;
					this->textBox15->Text = ""; this->textBox15->Enabled = false;
					this->textBox17->Text = ""; this->textBox17->Enabled = false;
					this->textBox18->Text = ""; this->textBox18->Enabled = false;
					this->textBox19->Text = ""; this->textBox19->Enabled = false;
					this->textBox20->Text = ""; this->textBox20->Enabled = false;
					this->textBox21->Text = ""; this->textBox21->Enabled = false;
					this->textBox22->Text = ""; this->textBox22->Enabled = false;
					this->textBox23->Text = ""; this->textBox23->Enabled = false;
					this->textBox24->Text = ""; this->textBox24->Enabled = false;
					this->textBox25->Text = ""; this->textBox25->Enabled = false;
					// Enable INDEX and set to default
					this->textBox5->Text = "6";
					this->textBox5->Enabled = true;
					this->P27IndexChanged(NULL,NULL);
					// Enable TRANSMIT button
					this->button64->Enabled = true;
					break;

				case 9: // V71 LAMBERT TARGET
					// Set labels
					this->label6->Text  = "ADDRESS"; this->label7->Text  = "TIG LF";
					this->label10->Text = "TIG RT";  this->label9->Text  = "RTx LF";
					this->label12->Text = "RTx RT";  this->label11->Text = "RTy LF";
					this->label14->Text = "RTy RT";  this->label13->Text = "RTz LF";
					this->label16->Text = "RTz RT";  this->label15->Text = "DT4 LF";
					this->label26->Text = "DT4 RT";  this->label25->Text = "ECSTEER";
					this->label24->Text = "UNUSED";  this->label23->Text = "UNUSED";
					this->label22->Text = "UNUSED";  this->label21->Text = "UNUSED";
					this->label20->Text = "UNUSED";  this->label19->Text = "UNUSED";
					this->label18->Text = "UNUSED"; 
					// Default number fields
					this->textBox6->Text = "3412"; this->textBox6->Enabled = false;
					this->textBox7->Text = "";  this->textBox7->Enabled = false;
					this->textBox8->Text = "";  this->textBox8->Enabled = false;
					this->textBox9->Text = "";  this->textBox9->Enabled = false;
					this->textBox10->Text = ""; this->textBox10->Enabled = false;
					this->textBox11->Text = ""; this->textBox11->Enabled = false;
					this->textBox12->Text = ""; this->textBox12->Enabled = false;
					this->textBox13->Text = ""; this->textBox13->Enabled = false;
					this->textBox14->Text = ""; this->textBox14->Enabled = false;
					this->textBox15->Text = ""; this->textBox15->Enabled = false;
					this->textBox17->Text = ""; this->textBox17->Enabled = false;
					this->textBox18->Text = ""; this->textBox18->Enabled = false;
					this->textBox19->Text = ""; this->textBox19->Enabled = false;
					this->textBox20->Text = ""; this->textBox20->Enabled = false;
					this->textBox21->Text = ""; this->textBox21->Enabled = false;
					this->textBox22->Text = ""; this->textBox22->Enabled = false;
					this->textBox23->Text = ""; this->textBox23->Enabled = false;
					this->textBox24->Text = ""; this->textBox24->Enabled = false;
					this->textBox25->Text = ""; this->textBox25->Enabled = false;
					// Enable INDEX and set to default
					this->textBox5->Text = "13";
					this->textBox5->Enabled = true;
					this->P27IndexChanged(NULL,NULL);
					// Enable TRANSMIT button
					this->button64->Enabled = true;
					break;

				case 10: // V71 LUNAR LANDING SITE
					// Set labels
					this->label6->Text  = "ADDRESS"; this->label7->Text  = "LSx LF";
					this->label10->Text = "LSx RT";  this->label9->Text  = "LSy LF";
					this->label12->Text = "LSy RT";  this->label11->Text = "LSz LF";
					this->label14->Text = "LSz RT";  this->label13->Text = "UNUSED";
					this->label16->Text = "UNUSED";  this->label15->Text = "UNUSED";
					this->label26->Text = "UNUSED";  this->label25->Text = "UNUSED";
					this->label24->Text = "UNUSED";  this->label23->Text = "UNUSED";
					this->label22->Text = "UNUSED";  this->label21->Text = "UNUSED";
					this->label20->Text = "UNUSED";  this->label19->Text = "UNUSED";
					this->label18->Text = "UNUSED"; 
					// Default number fields
					this->textBox6->Text = "2025"; this->textBox6->Enabled = false;
					this->textBox7->Text = "";  this->textBox7->Enabled = false;
					this->textBox8->Text = "";  this->textBox8->Enabled = false;
					this->textBox9->Text = "";  this->textBox9->Enabled = false;
					this->textBox10->Text = ""; this->textBox10->Enabled = false;
					this->textBox11->Text = ""; this->textBox11->Enabled = false;
					this->textBox12->Text = ""; this->textBox12->Enabled = false;
					this->textBox13->Text = ""; this->textBox13->Enabled = false;
					this->textBox14->Text = ""; this->textBox14->Enabled = false;
					this->textBox15->Text = ""; this->textBox15->Enabled = false;
					this->textBox17->Text = ""; this->textBox17->Enabled = false;
					this->textBox18->Text = ""; this->textBox18->Enabled = false;
					this->textBox19->Text = ""; this->textBox19->Enabled = false;
					this->textBox20->Text = ""; this->textBox20->Enabled = false;
					this->textBox21->Text = ""; this->textBox21->Enabled = false;
					this->textBox22->Text = ""; this->textBox22->Enabled = false;
					this->textBox23->Text = ""; this->textBox23->Enabled = false;
					this->textBox24->Text = ""; this->textBox24->Enabled = false;
					this->textBox25->Text = ""; this->textBox25->Enabled = false;
					// Enable INDEX and set to default
					this->textBox5->Text = "13";
					this->textBox5->Enabled = true;
					this->P27IndexChanged(NULL,NULL);
					// Enable TRANSMIT button
					this->button64->Enabled = true;
					break;

				case 11: // V72 NONCONTIGUOUS ERASABLE UPDATE
					// Set labels
					this->label6->Text  = "ADDR 1";  this->label7->Text  = "DATA 1";
					this->label10->Text = "ADDR 2";  this->label9->Text  = "DATA 2";
					this->label12->Text = "ADDR 3";  this->label11->Text = "DATA 3";
					this->label14->Text = "ADDR 4";  this->label13->Text = "DATA 4";
					this->label16->Text = "ADDR 5";  this->label15->Text = "DATA 5";
					this->label26->Text = "ADDR 6";  this->label25->Text = "DATA 6";
					this->label24->Text = "ADDR 7";  this->label23->Text = "DATA 7";
					this->label22->Text = "ADDR 8";  this->label21->Text = "DATA 8";
					this->label20->Text = "ADDR 9";  this->label19->Text = "DATA 9";
					this->label18->Text = "SPARE"; 
					// Default number fields
					this->textBox6->Text = ""; this->textBox6->Enabled = false;
					this->textBox7->Text = "";  this->textBox7->Enabled = false;
					this->textBox8->Text = "";  this->textBox8->Enabled = false;
					this->textBox9->Text = "";  this->textBox9->Enabled = false;
					this->textBox10->Text = ""; this->textBox10->Enabled = false;
					this->textBox11->Text = ""; this->textBox11->Enabled = false;
					this->textBox12->Text = ""; this->textBox12->Enabled = false;
					this->textBox13->Text = ""; this->textBox13->Enabled = false;
					this->textBox14->Text = ""; this->textBox14->Enabled = false;
					this->textBox15->Text = ""; this->textBox15->Enabled = false;
					this->textBox17->Text = ""; this->textBox17->Enabled = false;
					this->textBox18->Text = ""; this->textBox18->Enabled = false;
					this->textBox19->Text = ""; this->textBox19->Enabled = false;
					this->textBox20->Text = ""; this->textBox20->Enabled = false;
					this->textBox21->Text = ""; this->textBox21->Enabled = false;
					this->textBox22->Text = ""; this->textBox22->Enabled = false;
					this->textBox23->Text = ""; this->textBox23->Enabled = false;
					this->textBox24->Text = ""; this->textBox24->Enabled = false;
					this->textBox25->Text = ""; this->textBox25->Enabled = false;
					// Enable INDEX and set to default
					this->textBox5->Text = "3";
					this->textBox5->Enabled = true;
					this->P27IndexChanged(NULL,NULL);
					// Enable TRANSMIT button
					this->button64->Enabled = true;
					break;

				default:
				case 0: // DEFAULT
					// Set labels
					this->label6->Text  = "ADDR 1";  this->label7->Text  = "DATA 1";
					this->label10->Text = "ADDR 2";  this->label9->Text  = "DATA 2";
					this->label12->Text = "ADDR 3";  this->label11->Text = "DATA 3";
					this->label14->Text = "ADDR 4";  this->label13->Text = "DATA 4";
					this->label16->Text = "ADDR 5";  this->label15->Text = "DATA 5";
					this->label26->Text = "ADDR 6";  this->label25->Text = "DATA 6";
					this->label24->Text = "ADDR 7";  this->label23->Text = "DATA 7";
					this->label22->Text = "ADDR 8";  this->label21->Text = "DATA 8";
					this->label20->Text = "ADDR 9";  this->label19->Text = "DATA 9";
					this->label18->Text = "SPARE"; 
					// Reset inputs and block
					this->textBox5->Text = "XX";
					this->textBox5->Enabled = false;
					this->textBox6->Text = "XXXXX";  this->textBox6->Enabled = false;
					this->textBox7->Text = "XXXXX";  this->textBox7->Enabled = false;
					this->textBox8->Text = "XXXXX";  this->textBox8->Enabled = false;
					this->textBox9->Text = "XXXXX";  this->textBox9->Enabled = false;
					this->textBox10->Text = "XXXXX"; this->textBox10->Enabled = false;
					this->textBox11->Text = "XXXXX"; this->textBox11->Enabled = false;
					this->textBox12->Text = "XXXXX"; this->textBox12->Enabled = false;
					this->textBox13->Text = "XXXXX"; this->textBox13->Enabled = false;
					this->textBox14->Text = "XXXXX"; this->textBox14->Enabled = false;
					this->textBox15->Text = "XXXXX"; this->textBox15->Enabled = false;
					this->textBox17->Text = "XXXXX"; this->textBox17->Enabled = false;
					this->textBox18->Text = "XXXXX"; this->textBox18->Enabled = false;
					this->textBox19->Text = "XXXXX"; this->textBox19->Enabled = false;
					this->textBox20->Text = "XXXXX"; this->textBox20->Enabled = false;
					this->textBox21->Text = "XXXXX"; this->textBox21->Enabled = false;
					this->textBox22->Text = "XXXXX"; this->textBox22->Enabled = false;
					this->textBox23->Text = "XXXXX"; this->textBox23->Enabled = false;
					this->textBox24->Text = "XXXXX"; this->textBox24->Enabled = false;
					this->textBox25->Text = "XXXXX"; this->textBox25->Enabled = false;
					// Reset status
					this->textBox26->Text = "INACTIVE"; this->textBox25->Enabled = false;
					// And disable TRANSMIT button
					this->button64->Enabled = false;
					break;
			}

			
		}

		public: 
		void send_agc_key(char key){
			int bytesXmit = SOCKET_ERROR;
			unsigned char cmdbuf[4];
			cmdbuf[0] = 043; // VA,SA
			switch(key){
				case 'V': // 11-000-101 11-010-001										
					cmdbuf[1] = 0305;
					cmdbuf[2] = 0321;
					break;
				case 'N': // 11-111-100 00-011-111
					cmdbuf[1] = 0374;
					cmdbuf[2] = 0037;
					break;
				case 'E': // 11-110-000 01-111-100
					cmdbuf[1] = 0360;
					cmdbuf[2] = 0174;
					break;
				case 'R': // 11-001-001 10-110-010
					cmdbuf[1] = 0311;
					cmdbuf[2] = 0262;
					break;
				case 'C': // 11-111-000 00-111-110
					cmdbuf[1] = 0370;
					cmdbuf[2] = 0076;
					break;
				case 'K': // 11-100-100 11-011-001
					cmdbuf[1] = 0344;
					cmdbuf[2] = 0331;
					break;
				case '+': // 11-101-000 10-111-010
					cmdbuf[1] = 0350;
					cmdbuf[2] = 0372;
					break;
				case '-': // 11-101-100 10-011-011
					cmdbuf[1] = 0354;
					cmdbuf[2] = 0233;
					break;
				case '1': // 10-000-111 11-000-001
					cmdbuf[1] = 0207;
					cmdbuf[2] = 0301;
					break;
				case '2': // 10-001-011 10-100-010
					cmdbuf[1] = 0213;
					cmdbuf[2] = 0242;
					break;
				case '3': // 10-001-111 10-000-011
					cmdbuf[1] = 0217;
					cmdbuf[2] = 0203;
					break;
				case '4': // 10-010-011 01-100-100
					cmdbuf[1] = 0223;
					cmdbuf[2] = 0144;
					break;
				case '5': // 10-010-111 01-000-101
					cmdbuf[1] = 0227;
					cmdbuf[2] = 0105;
					break; 
				case '6': // 10-011-011 00-100-110
					cmdbuf[1] = 0233;
					cmdbuf[2] = 0046;
					break;
				case '7': // 10-011-111 00-000-111
					cmdbuf[1] = 0237;
					cmdbuf[2] = 0007;
					break;
				case '8': // 10-100-010 11-101-000
					cmdbuf[1] = 0242;
					cmdbuf[2] = 0350;
					break;
				case '9': // 10-100-110 11-001-001
					cmdbuf[1] = 0246;
					cmdbuf[2] = 0311;
					break;
				case '0': // 11-000-001 11-110-000
					cmdbuf[1] = 0301;
					cmdbuf[2] = 0360;
					break;
			}
 			bytesXmit = send(*m_socket,(char *)cmdbuf,3,0);
			
		}
	// POINTER TO SOCKET FOR IO
	SOCKET * m_socket;
	// CMC UPLINK PROCESS
	int cmc_uplink_state;

	private: System::Windows::Forms::GroupBox *  groupBox1;
	private: System::Windows::Forms::Button *  Button1;
	private: System::Windows::Forms::Button *  button2;
	private: System::Windows::Forms::GroupBox *  groupBox2;
	private: System::Windows::Forms::GroupBox *  groupBox3;
	private: System::Windows::Forms::Button *  button3;
	private: System::Windows::Forms::Button *  button4;
	private: System::Windows::Forms::GroupBox *  groupBox4;
	private: System::Windows::Forms::Button *  button5;
	private: System::Windows::Forms::Button *  button6;
	private: System::Windows::Forms::GroupBox *  groupBox5;
	private: System::Windows::Forms::Button *  button7;
	private: System::Windows::Forms::Button *  button8;
	private: System::Windows::Forms::GroupBox *  groupBox6;
	private: System::Windows::Forms::Button *  button9;
	private: System::Windows::Forms::Button *  button10;
	private: System::Windows::Forms::GroupBox *  groupBox7;
	private: System::Windows::Forms::Button *  button11;
	private: System::Windows::Forms::Button *  button12;
	private: System::Windows::Forms::Button *  button13;
	private: System::Windows::Forms::GroupBox *  groupBox8;
	private: System::Windows::Forms::Button *  button14;
	private: System::Windows::Forms::Button *  button15;
	private: System::Windows::Forms::Button *  button16;
	private: System::Windows::Forms::GroupBox *  groupBox9;
	private: System::Windows::Forms::Button *  button17;
	private: System::Windows::Forms::Button *  button18;
	private: System::Windows::Forms::Button *  button19;
	private: System::Windows::Forms::Button *  button20;
	private: System::Windows::Forms::GroupBox *  groupBox10;
	private: System::Windows::Forms::Button *  button21;
	private: System::Windows::Forms::Button *  button22;
	private: System::Windows::Forms::Button *  button23;
	private: System::Windows::Forms::Button *  button24;
	private: System::Windows::Forms::GroupBox *  groupBox11;
	private: System::Windows::Forms::Button *  button25;
	private: System::Windows::Forms::Button *  button26;
	private: System::Windows::Forms::Button *  button27;
	private: System::Windows::Forms::GroupBox *  groupBox12;
	private: System::Windows::Forms::Button *  button28;
	private: System::Windows::Forms::Button *  button29;
	private: System::Windows::Forms::Button *  button30;
	private: System::Windows::Forms::Button *  button31;
	private: System::Windows::Forms::GroupBox *  groupBox13;
	private: System::Windows::Forms::Button *  button32;
	private: System::Windows::Forms::Button *  button33;
	private: System::Windows::Forms::Button *  button34;
	private: System::Windows::Forms::GroupBox *  groupBox14;
	private: System::Windows::Forms::Button *  button35;
	private: System::Windows::Forms::Button *  button36;
	private: System::Windows::Forms::Button *  button37;
	private: System::Windows::Forms::Button *  button38;
	private: System::Windows::Forms::GroupBox *  groupBox15;
	private: System::Windows::Forms::Button *  button39;
	private: System::Windows::Forms::Button *  button40;
	private: System::Windows::Forms::GroupBox *  groupBox16;
	private: System::Windows::Forms::Button *  button41;
	private: System::Windows::Forms::Button *  button42;
	private: System::Windows::Forms::GroupBox *  groupBox17;
	private: System::Windows::Forms::Button *  button43;
	private: System::Windows::Forms::Button *  button44;
	private: System::Windows::Forms::GroupBox *  groupBox18;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::TextBox *  textBox1;
	private: System::Windows::Forms::TextBox *  textBox2;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::TextBox *  textBox3;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::TextBox *  textBox4;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::Button *  button45;
	private: System::Windows::Forms::GroupBox *  groupBox19;
	private: System::Windows::Forms::Label *  label5;
public: System::Windows::Forms::TextBox *  textBox5;

	private: System::Windows::Forms::GroupBox *  groupBox20;
	public:  System::Windows::Forms::TextBox *  textBox6;
	private: System::Windows::Forms::Label *  label6;
	public:  System::Windows::Forms::TextBox *  textBox7;
	private: System::Windows::Forms::Label *  label7;
	public:  System::Windows::Forms::ComboBox *  comboBox1;
private: System::Windows::Forms::GroupBox *  groupBox21;
public: System::Windows::Forms::Label *  label75;
public: System::Windows::Forms::Label *  label70;
public: System::Windows::Forms::Label *  label69;
public: System::Windows::Forms::TextBox *  textBox137;
private: System::Windows::Forms::Label *  label138;
public: System::Windows::Forms::TextBox *  textBox136;
private: System::Windows::Forms::Label *  label137;
public: System::Windows::Forms::TextBox *  textBox135;
private: System::Windows::Forms::Label *  label136;
public: System::Windows::Forms::TextBox *  textBox134;
private: System::Windows::Forms::Label *  label135;
public: System::Windows::Forms::TextBox *  textBox133;
private: System::Windows::Forms::Label *  label131;
public: System::Windows::Forms::TextBox *  textBox129;
private: System::Windows::Forms::Label *  label8;
public: System::Windows::Forms::Label *  label72;
public: System::Windows::Forms::TextBox *  textBox8;

private: System::Windows::Forms::Label *  label9;
public: System::Windows::Forms::TextBox *  textBox9;

private: System::Windows::Forms::Label *  label10;
public: System::Windows::Forms::TextBox *  textBox10;

private: System::Windows::Forms::Label *  label11;
public: System::Windows::Forms::TextBox *  textBox11;

private: System::Windows::Forms::Label *  label12;
public: System::Windows::Forms::TextBox *  textBox12;

private: System::Windows::Forms::Label *  label13;
public: System::Windows::Forms::TextBox *  textBox13;

private: System::Windows::Forms::Label *  label14;
public: System::Windows::Forms::TextBox *  textBox14;

private: System::Windows::Forms::Label *  label15;
public: System::Windows::Forms::TextBox *  textBox15;

private: System::Windows::Forms::Label *  label16;
public: System::Windows::Forms::TextBox *  textBox18;





private: System::Windows::Forms::Label *  label19;
public: System::Windows::Forms::TextBox *  textBox19;

private: System::Windows::Forms::Label *  label20;
public: System::Windows::Forms::TextBox *  textBox20;

private: System::Windows::Forms::Label *  label21;
public: System::Windows::Forms::TextBox *  textBox21;

private: System::Windows::Forms::Label *  label22;
public: System::Windows::Forms::TextBox *  textBox22;

private: System::Windows::Forms::Label *  label23;
public: System::Windows::Forms::TextBox *  textBox23;

private: System::Windows::Forms::Label *  label24;
public: System::Windows::Forms::TextBox *  textBox24;

private: System::Windows::Forms::Label *  label25;
public: System::Windows::Forms::TextBox *  textBox25;

private: System::Windows::Forms::Label *  label26;
private: System::Windows::Forms::Button *  button46;
private: System::Windows::Forms::Button *  button47;
private: System::Windows::Forms::Button *  button48;
private: System::Windows::Forms::Button *  button49;
private: System::Windows::Forms::Button *  button50;
private: System::Windows::Forms::Button *  button51;
private: System::Windows::Forms::Button *  button52;
private: System::Windows::Forms::Button *  button53;
private: System::Windows::Forms::Button *  button54;
private: System::Windows::Forms::Button *  button55;
private: System::Windows::Forms::Button *  button56;
private: System::Windows::Forms::Button *  button57;
private: System::Windows::Forms::Button *  button58;
private: System::Windows::Forms::Button *  button59;
private: System::Windows::Forms::Button *  button60;
private: System::Windows::Forms::Button *  button61;
private: System::Windows::Forms::Button *  button62;
private: System::Windows::Forms::Button *  button63;
private: System::Windows::Forms::Label *  label27;
public:  System::Windows::Forms::TextBox *  textBox26;
public:  System::Windows::Forms::Button *  button64;
public: System::Windows::Forms::TextBox *  textBox17;



private: System::Windows::Forms::Label *  label18;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->groupBox16 = new System::Windows::Forms::GroupBox();
			this->button41 = new System::Windows::Forms::Button();
			this->button42 = new System::Windows::Forms::Button();
			this->groupBox15 = new System::Windows::Forms::GroupBox();
			this->button39 = new System::Windows::Forms::Button();
			this->button40 = new System::Windows::Forms::Button();
			this->groupBox14 = new System::Windows::Forms::GroupBox();
			this->button35 = new System::Windows::Forms::Button();
			this->button36 = new System::Windows::Forms::Button();
			this->button37 = new System::Windows::Forms::Button();
			this->button38 = new System::Windows::Forms::Button();
			this->groupBox13 = new System::Windows::Forms::GroupBox();
			this->button32 = new System::Windows::Forms::Button();
			this->button33 = new System::Windows::Forms::Button();
			this->button34 = new System::Windows::Forms::Button();
			this->groupBox12 = new System::Windows::Forms::GroupBox();
			this->button28 = new System::Windows::Forms::Button();
			this->button29 = new System::Windows::Forms::Button();
			this->button30 = new System::Windows::Forms::Button();
			this->button31 = new System::Windows::Forms::Button();
			this->groupBox11 = new System::Windows::Forms::GroupBox();
			this->button25 = new System::Windows::Forms::Button();
			this->button26 = new System::Windows::Forms::Button();
			this->button27 = new System::Windows::Forms::Button();
			this->groupBox10 = new System::Windows::Forms::GroupBox();
			this->button21 = new System::Windows::Forms::Button();
			this->button22 = new System::Windows::Forms::Button();
			this->button23 = new System::Windows::Forms::Button();
			this->button24 = new System::Windows::Forms::Button();
			this->groupBox9 = new System::Windows::Forms::GroupBox();
			this->button20 = new System::Windows::Forms::Button();
			this->button17 = new System::Windows::Forms::Button();
			this->button18 = new System::Windows::Forms::Button();
			this->button19 = new System::Windows::Forms::Button();
			this->groupBox8 = new System::Windows::Forms::GroupBox();
			this->button14 = new System::Windows::Forms::Button();
			this->button15 = new System::Windows::Forms::Button();
			this->button16 = new System::Windows::Forms::Button();
			this->groupBox7 = new System::Windows::Forms::GroupBox();
			this->button13 = new System::Windows::Forms::Button();
			this->button11 = new System::Windows::Forms::Button();
			this->button12 = new System::Windows::Forms::Button();
			this->groupBox6 = new System::Windows::Forms::GroupBox();
			this->button9 = new System::Windows::Forms::Button();
			this->button10 = new System::Windows::Forms::Button();
			this->groupBox5 = new System::Windows::Forms::GroupBox();
			this->button7 = new System::Windows::Forms::Button();
			this->button8 = new System::Windows::Forms::Button();
			this->groupBox4 = new System::Windows::Forms::GroupBox();
			this->button5 = new System::Windows::Forms::Button();
			this->button6 = new System::Windows::Forms::Button();
			this->groupBox3 = new System::Windows::Forms::GroupBox();
			this->button3 = new System::Windows::Forms::Button();
			this->button4 = new System::Windows::Forms::Button();
			this->groupBox2 = new System::Windows::Forms::GroupBox();
			this->Button1 = new System::Windows::Forms::Button();
			this->button2 = new System::Windows::Forms::Button();
			this->groupBox17 = new System::Windows::Forms::GroupBox();
			this->button43 = new System::Windows::Forms::Button();
			this->button44 = new System::Windows::Forms::Button();
			this->groupBox18 = new System::Windows::Forms::GroupBox();
			this->button45 = new System::Windows::Forms::Button();
			this->textBox4 = new System::Windows::Forms::TextBox();
			this->label4 = new System::Windows::Forms::Label();
			this->textBox3 = new System::Windows::Forms::TextBox();
			this->label3 = new System::Windows::Forms::Label();
			this->textBox2 = new System::Windows::Forms::TextBox();
			this->label2 = new System::Windows::Forms::Label();
			this->textBox1 = new System::Windows::Forms::TextBox();
			this->label1 = new System::Windows::Forms::Label();
			this->groupBox19 = new System::Windows::Forms::GroupBox();
			this->groupBox21 = new System::Windows::Forms::GroupBox();
			this->button63 = new System::Windows::Forms::Button();
			this->button62 = new System::Windows::Forms::Button();
			this->button61 = new System::Windows::Forms::Button();
			this->button58 = new System::Windows::Forms::Button();
			this->button59 = new System::Windows::Forms::Button();
			this->button60 = new System::Windows::Forms::Button();
			this->button55 = new System::Windows::Forms::Button();
			this->button56 = new System::Windows::Forms::Button();
			this->button57 = new System::Windows::Forms::Button();
			this->button54 = new System::Windows::Forms::Button();
			this->button53 = new System::Windows::Forms::Button();
			this->button52 = new System::Windows::Forms::Button();
			this->button51 = new System::Windows::Forms::Button();
			this->button50 = new System::Windows::Forms::Button();
			this->button49 = new System::Windows::Forms::Button();
			this->button48 = new System::Windows::Forms::Button();
			this->button47 = new System::Windows::Forms::Button();
			this->button46 = new System::Windows::Forms::Button();
			this->label75 = new System::Windows::Forms::Label();
			this->label70 = new System::Windows::Forms::Label();
			this->label69 = new System::Windows::Forms::Label();
			this->textBox137 = new System::Windows::Forms::TextBox();
			this->label138 = new System::Windows::Forms::Label();
			this->textBox136 = new System::Windows::Forms::TextBox();
			this->label137 = new System::Windows::Forms::Label();
			this->textBox135 = new System::Windows::Forms::TextBox();
			this->label136 = new System::Windows::Forms::Label();
			this->textBox134 = new System::Windows::Forms::TextBox();
			this->label135 = new System::Windows::Forms::Label();
			this->textBox133 = new System::Windows::Forms::TextBox();
			this->label131 = new System::Windows::Forms::Label();
			this->textBox129 = new System::Windows::Forms::TextBox();
			this->label8 = new System::Windows::Forms::Label();
			this->label72 = new System::Windows::Forms::Label();
			this->groupBox20 = new System::Windows::Forms::GroupBox();
			this->textBox17 = new System::Windows::Forms::TextBox();
			this->label18 = new System::Windows::Forms::Label();
			this->button64 = new System::Windows::Forms::Button();
			this->textBox26 = new System::Windows::Forms::TextBox();
			this->label27 = new System::Windows::Forms::Label();
			this->textBox18 = new System::Windows::Forms::TextBox();
			this->label19 = new System::Windows::Forms::Label();
			this->textBox19 = new System::Windows::Forms::TextBox();
			this->label20 = new System::Windows::Forms::Label();
			this->textBox20 = new System::Windows::Forms::TextBox();
			this->label21 = new System::Windows::Forms::Label();
			this->textBox21 = new System::Windows::Forms::TextBox();
			this->label22 = new System::Windows::Forms::Label();
			this->textBox22 = new System::Windows::Forms::TextBox();
			this->label23 = new System::Windows::Forms::Label();
			this->textBox23 = new System::Windows::Forms::TextBox();
			this->label24 = new System::Windows::Forms::Label();
			this->textBox24 = new System::Windows::Forms::TextBox();
			this->label25 = new System::Windows::Forms::Label();
			this->textBox25 = new System::Windows::Forms::TextBox();
			this->label26 = new System::Windows::Forms::Label();
			this->textBox14 = new System::Windows::Forms::TextBox();
			this->label15 = new System::Windows::Forms::Label();
			this->textBox15 = new System::Windows::Forms::TextBox();
			this->label16 = new System::Windows::Forms::Label();
			this->textBox12 = new System::Windows::Forms::TextBox();
			this->label13 = new System::Windows::Forms::Label();
			this->textBox13 = new System::Windows::Forms::TextBox();
			this->label14 = new System::Windows::Forms::Label();
			this->textBox10 = new System::Windows::Forms::TextBox();
			this->label11 = new System::Windows::Forms::Label();
			this->textBox11 = new System::Windows::Forms::TextBox();
			this->label12 = new System::Windows::Forms::Label();
			this->textBox8 = new System::Windows::Forms::TextBox();
			this->label9 = new System::Windows::Forms::Label();
			this->textBox9 = new System::Windows::Forms::TextBox();
			this->label10 = new System::Windows::Forms::Label();
			this->comboBox1 = new System::Windows::Forms::ComboBox();
			this->textBox7 = new System::Windows::Forms::TextBox();
			this->label7 = new System::Windows::Forms::Label();
			this->textBox6 = new System::Windows::Forms::TextBox();
			this->label6 = new System::Windows::Forms::Label();
			this->textBox5 = new System::Windows::Forms::TextBox();
			this->label5 = new System::Windows::Forms::Label();
			this->groupBox1->SuspendLayout();
			this->groupBox16->SuspendLayout();
			this->groupBox15->SuspendLayout();
			this->groupBox14->SuspendLayout();
			this->groupBox13->SuspendLayout();
			this->groupBox12->SuspendLayout();
			this->groupBox11->SuspendLayout();
			this->groupBox10->SuspendLayout();
			this->groupBox9->SuspendLayout();
			this->groupBox8->SuspendLayout();
			this->groupBox7->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox17->SuspendLayout();
			this->groupBox18->SuspendLayout();
			this->groupBox19->SuspendLayout();
			this->groupBox21->SuspendLayout();
			this->groupBox20->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->groupBox16);
			this->groupBox1->Controls->Add(this->groupBox15);
			this->groupBox1->Controls->Add(this->groupBox14);
			this->groupBox1->Controls->Add(this->groupBox13);
			this->groupBox1->Controls->Add(this->groupBox12);
			this->groupBox1->Controls->Add(this->groupBox11);
			this->groupBox1->Controls->Add(this->groupBox10);
			this->groupBox1->Controls->Add(this->groupBox9);
			this->groupBox1->Controls->Add(this->groupBox8);
			this->groupBox1->Controls->Add(this->groupBox7);
			this->groupBox1->Controls->Add(this->groupBox6);
			this->groupBox1->Controls->Add(this->groupBox5);
			this->groupBox1->Controls->Add(this->groupBox4);
			this->groupBox1->Controls->Add(this->groupBox3);
			this->groupBox1->Controls->Add(this->groupBox2);
			this->groupBox1->Location = System::Drawing::Point(8, 8);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(352, 504);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"REALTIME COMMANDS";
			// 
			// groupBox16
			// 
			this->groupBox16->Controls->Add(this->button41);
			this->groupBox16->Controls->Add(this->button42);
			this->groupBox16->Location = System::Drawing::Point(120, 160);
			this->groupBox16->Name = S"groupBox16";
			this->groupBox16->Size = System::Drawing::Size(112, 72);
			this->groupBox16->TabIndex = 34;
			this->groupBox16->TabStop = false;
			this->groupBox16->Text = S"R/T CMD SALVO";
			// 
			// button41
			// 
			this->button41->Location = System::Drawing::Point(8, 16);
			this->button41->Name = S"button41";
			this->button41->Size = System::Drawing::Size(96, 23);
			this->button41->TabIndex = 16;
			this->button41->Text = S"RESET 1";
			this->button41->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button41_Click);
			// 
			// button42
			// 
			this->button42->Location = System::Drawing::Point(8, 40);
			this->button42->Name = S"button42";
			this->button42->Size = System::Drawing::Size(96, 23);
			this->button42->TabIndex = 17;
			this->button42->Text = S"RESET 2";
			this->button42->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button42_Click);
			// 
			// groupBox15
			// 
			this->groupBox15->Controls->Add(this->button39);
			this->groupBox15->Controls->Add(this->button40);
			this->groupBox15->Location = System::Drawing::Point(232, 88);
			this->groupBox15->Name = S"groupBox15";
			this->groupBox15->Size = System::Drawing::Size(112, 72);
			this->groupBox15->TabIndex = 33;
			this->groupBox15->TabStop = false;
			this->groupBox15->Text = S"ANTENNA SEL";
			// 
			// button39
			// 
			this->button39->Location = System::Drawing::Point(8, 16);
			this->button39->Name = S"button39";
			this->button39->Size = System::Drawing::Size(96, 23);
			this->button39->TabIndex = 16;
			this->button39->Text = S"RESET";
			this->button39->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button39_Click);
			// 
			// button40
			// 
			this->button40->Location = System::Drawing::Point(8, 40);
			this->button40->Name = S"button40";
			this->button40->Size = System::Drawing::Size(96, 23);
			this->button40->TabIndex = 17;
			this->button40->Text = S"D OMNI";
			this->button40->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button40_Click);
			// 
			// groupBox14
			// 
			this->groupBox14->Controls->Add(this->button35);
			this->groupBox14->Controls->Add(this->button36);
			this->groupBox14->Controls->Add(this->button37);
			this->groupBox14->Controls->Add(this->button38);
			this->groupBox14->Location = System::Drawing::Point(232, 376);
			this->groupBox14->Name = S"groupBox14";
			this->groupBox14->Size = System::Drawing::Size(112, 120);
			this->groupBox14->TabIndex = 32;
			this->groupBox14->TabStop = false;
			this->groupBox14->Text = S"DSE TAPE CTL";
			// 
			// button35
			// 
			this->button35->Location = System::Drawing::Point(8, 88);
			this->button35->Name = S"button35";
			this->button35->Size = System::Drawing::Size(96, 23);
			this->button35->TabIndex = 19;
			this->button35->Text = S"REWIND";
			this->button35->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button35_Click);
			// 
			// button36
			// 
			this->button36->Location = System::Drawing::Point(8, 16);
			this->button36->Name = S"button36";
			this->button36->Size = System::Drawing::Size(96, 23);
			this->button36->TabIndex = 18;
			this->button36->Text = S"RESET";
			this->button36->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button36_Click);
			// 
			// button37
			// 
			this->button37->Location = System::Drawing::Point(8, 40);
			this->button37->Name = S"button37";
			this->button37->Size = System::Drawing::Size(96, 23);
			this->button37->TabIndex = 16;
			this->button37->Text = S"FORWARD";
			this->button37->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button37_Click);
			// 
			// button38
			// 
			this->button38->Location = System::Drawing::Point(8, 64);
			this->button38->Name = S"button38";
			this->button38->Size = System::Drawing::Size(96, 23);
			this->button38->TabIndex = 17;
			this->button38->Text = S"STOP";
			this->button38->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button38_Click);
			// 
			// groupBox13
			// 
			this->groupBox13->Controls->Add(this->button32);
			this->groupBox13->Controls->Add(this->button33);
			this->groupBox13->Controls->Add(this->button34);
			this->groupBox13->Location = System::Drawing::Point(8, 256);
			this->groupBox13->Name = S"groupBox13";
			this->groupBox13->Size = System::Drawing::Size(112, 96);
			this->groupBox13->TabIndex = 31;
			this->groupBox13->TabStop = false;
			this->groupBox13->Text = S"PCM BITRATE";
			// 
			// button32
			// 
			this->button32->Location = System::Drawing::Point(8, 16);
			this->button32->Name = S"button32";
			this->button32->Size = System::Drawing::Size(96, 23);
			this->button32->TabIndex = 18;
			this->button32->Text = S"RESET";
			this->button32->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button32_Click);
			// 
			// button33
			// 
			this->button33->Location = System::Drawing::Point(8, 40);
			this->button33->Name = S"button33";
			this->button33->Size = System::Drawing::Size(96, 23);
			this->button33->TabIndex = 16;
			this->button33->Text = S"HIGH";
			this->button33->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button33_Click);
			// 
			// button34
			// 
			this->button34->Location = System::Drawing::Point(8, 64);
			this->button34->Name = S"button34";
			this->button34->Size = System::Drawing::Size(96, 23);
			this->button34->TabIndex = 17;
			this->button34->Text = S"LOW";
			this->button34->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button34_Click);
			// 
			// groupBox12
			// 
			this->groupBox12->Controls->Add(this->button28);
			this->groupBox12->Controls->Add(this->button29);
			this->groupBox12->Controls->Add(this->button30);
			this->groupBox12->Controls->Add(this->button31);
			this->groupBox12->Location = System::Drawing::Point(232, 256);
			this->groupBox12->Name = S"groupBox12";
			this->groupBox12->Size = System::Drawing::Size(112, 120);
			this->groupBox12->TabIndex = 30;
			this->groupBox12->TabStop = false;
			this->groupBox12->Text = S"DSE ELECT";
			// 
			// button28
			// 
			this->button28->Location = System::Drawing::Point(8, 88);
			this->button28->Name = S"button28";
			this->button28->Size = System::Drawing::Size(96, 23);
			this->button28->TabIndex = 19;
			this->button28->Text = S"PLAYBACK";
			this->button28->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button28_Click);
			// 
			// button29
			// 
			this->button29->Location = System::Drawing::Point(8, 16);
			this->button29->Name = S"button29";
			this->button29->Size = System::Drawing::Size(96, 23);
			this->button29->TabIndex = 18;
			this->button29->Text = S"RESET";
			this->button29->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button29_Click);
			// 
			// button30
			// 
			this->button30->Location = System::Drawing::Point(8, 40);
			this->button30->Name = S"button30";
			this->button30->Size = System::Drawing::Size(96, 23);
			this->button30->TabIndex = 16;
			this->button30->Text = S"RECORD";
			this->button30->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button30_Click);
			// 
			// button31
			// 
			this->button31->Location = System::Drawing::Point(8, 64);
			this->button31->Name = S"button31";
			this->button31->Size = System::Drawing::Size(96, 23);
			this->button31->TabIndex = 17;
			this->button31->Text = S"OFF";
			this->button31->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button31_Click);
			// 
			// groupBox11
			// 
			this->groupBox11->Controls->Add(this->button25);
			this->groupBox11->Controls->Add(this->button26);
			this->groupBox11->Controls->Add(this->button27);
			this->groupBox11->Location = System::Drawing::Point(232, 160);
			this->groupBox11->Name = S"groupBox11";
			this->groupBox11->Size = System::Drawing::Size(112, 96);
			this->groupBox11->TabIndex = 29;
			this->groupBox11->TabStop = false;
			this->groupBox11->Text = S"DSE PLAYBACK";
			// 
			// button25
			// 
			this->button25->Location = System::Drawing::Point(8, 16);
			this->button25->Name = S"button25";
			this->button25->Size = System::Drawing::Size(96, 23);
			this->button25->TabIndex = 18;
			this->button25->Text = S"RESET";
			this->button25->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button25_Click);
			// 
			// button26
			// 
			this->button26->Location = System::Drawing::Point(8, 40);
			this->button26->Name = S"button26";
			this->button26->Size = System::Drawing::Size(96, 23);
			this->button26->TabIndex = 16;
			this->button26->Text = S"SELECT CSM";
			this->button26->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button26_Click);
			// 
			// button27
			// 
			this->button27->Location = System::Drawing::Point(8, 64);
			this->button27->Name = S"button27";
			this->button27->Size = System::Drawing::Size(96, 23);
			this->button27->TabIndex = 17;
			this->button27->Text = S"SELECT LM";
			this->button27->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button27_Click);
			// 
			// groupBox10
			// 
			this->groupBox10->Controls->Add(this->button21);
			this->groupBox10->Controls->Add(this->button22);
			this->groupBox10->Controls->Add(this->button23);
			this->groupBox10->Controls->Add(this->button24);
			this->groupBox10->Location = System::Drawing::Point(120, 376);
			this->groupBox10->Name = S"groupBox10";
			this->groupBox10->Size = System::Drawing::Size(112, 120);
			this->groupBox10->TabIndex = 28;
			this->groupBox10->TabStop = false;
			this->groupBox10->Text = S"POWER AMP";
			// 
			// button21
			// 
			this->button21->Location = System::Drawing::Point(8, 88);
			this->button21->Name = S"button21";
			this->button21->Size = System::Drawing::Size(96, 23);
			this->button21->TabIndex = 19;
			this->button21->Text = S"OFF";
			this->button21->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button21_Click);
			// 
			// button22
			// 
			this->button22->Location = System::Drawing::Point(8, 16);
			this->button22->Name = S"button22";
			this->button22->Size = System::Drawing::Size(96, 23);
			this->button22->TabIndex = 18;
			this->button22->Text = S"RESET";
			this->button22->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button22_Click);
			// 
			// button23
			// 
			this->button23->Location = System::Drawing::Point(8, 40);
			this->button23->Name = S"button23";
			this->button23->Size = System::Drawing::Size(96, 23);
			this->button23->TabIndex = 16;
			this->button23->Text = S"HIGH";
			this->button23->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button23_Click);
			// 
			// button24
			// 
			this->button24->Location = System::Drawing::Point(8, 64);
			this->button24->Name = S"button24";
			this->button24->Size = System::Drawing::Size(96, 23);
			this->button24->TabIndex = 17;
			this->button24->Text = S"LOW";
			this->button24->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button24_Click);
			// 
			// groupBox9
			// 
			this->groupBox9->Controls->Add(this->button20);
			this->groupBox9->Controls->Add(this->button17);
			this->groupBox9->Controls->Add(this->button18);
			this->groupBox9->Controls->Add(this->button19);
			this->groupBox9->Location = System::Drawing::Point(8, 376);
			this->groupBox9->Name = S"groupBox9";
			this->groupBox9->Size = System::Drawing::Size(112, 120);
			this->groupBox9->TabIndex = 27;
			this->groupBox9->TabStop = false;
			this->groupBox9->Text = S"UNIFIED S-BAND";
			// 
			// button20
			// 
			this->button20->Location = System::Drawing::Point(8, 88);
			this->button20->Name = S"button20";
			this->button20->Size = System::Drawing::Size(96, 23);
			this->button20->TabIndex = 19;
			this->button20->Text = S"FM/TAPE ON";
			this->button20->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button20_Click);
			// 
			// button17
			// 
			this->button17->Location = System::Drawing::Point(8, 16);
			this->button17->Name = S"button17";
			this->button17->Size = System::Drawing::Size(96, 23);
			this->button17->TabIndex = 18;
			this->button17->Text = S"MODE RESET";
			this->button17->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button17_Click);
			// 
			// button18
			// 
			this->button18->Location = System::Drawing::Point(8, 40);
			this->button18->Name = S"button18";
			this->button18->Size = System::Drawing::Size(96, 23);
			this->button18->TabIndex = 16;
			this->button18->Text = S"BU VOICE OFF";
			this->button18->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button18_Click);
			// 
			// button19
			// 
			this->button19->Location = System::Drawing::Point(8, 64);
			this->button19->Name = S"button19";
			this->button19->Size = System::Drawing::Size(96, 23);
			this->button19->TabIndex = 17;
			this->button19->Text = S"FM/TAPE OFF";
			this->button19->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button19_Click);
			// 
			// groupBox8
			// 
			this->groupBox8->Controls->Add(this->button14);
			this->groupBox8->Controls->Add(this->button15);
			this->groupBox8->Controls->Add(this->button16);
			this->groupBox8->Location = System::Drawing::Point(8, 160);
			this->groupBox8->Name = S"groupBox8";
			this->groupBox8->Size = System::Drawing::Size(112, 96);
			this->groupBox8->TabIndex = 26;
			this->groupBox8->TabStop = false;
			this->groupBox8->Text = S"R/T PCM";
			// 
			// button14
			// 
			this->button14->Location = System::Drawing::Point(8, 16);
			this->button14->Name = S"button14";
			this->button14->Size = System::Drawing::Size(96, 23);
			this->button14->TabIndex = 18;
			this->button14->Text = S"RESET";
			this->button14->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button14_Click);
			// 
			// button15
			// 
			this->button15->Location = System::Drawing::Point(8, 40);
			this->button15->Name = S"button15";
			this->button15->Size = System::Drawing::Size(96, 23);
			this->button15->TabIndex = 16;
			this->button15->Text = S"1024 ON";
			this->button15->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button15_Click);
			// 
			// button16
			// 
			this->button16->Location = System::Drawing::Point(8, 64);
			this->button16->Name = S"button16";
			this->button16->Size = System::Drawing::Size(96, 23);
			this->button16->TabIndex = 17;
			this->button16->Text = S"OFF";
			this->button16->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button16_Click);
			// 
			// groupBox7
			// 
			this->groupBox7->Controls->Add(this->button13);
			this->groupBox7->Controls->Add(this->button11);
			this->groupBox7->Controls->Add(this->button12);
			this->groupBox7->Location = System::Drawing::Point(120, 256);
			this->groupBox7->Name = S"groupBox7";
			this->groupBox7->Size = System::Drawing::Size(112, 96);
			this->groupBox7->TabIndex = 25;
			this->groupBox7->TabStop = false;
			this->groupBox7->Text = S"VHF RANGING";
			// 
			// button13
			// 
			this->button13->Location = System::Drawing::Point(8, 16);
			this->button13->Name = S"button13";
			this->button13->Size = System::Drawing::Size(96, 23);
			this->button13->TabIndex = 18;
			this->button13->Text = S"RESET";
			this->button13->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button13_Click);
			// 
			// button11
			// 
			this->button11->Location = System::Drawing::Point(8, 40);
			this->button11->Name = S"button11";
			this->button11->Size = System::Drawing::Size(96, 23);
			this->button11->TabIndex = 16;
			this->button11->Text = S"ENABLE";
			this->button11->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button11_Click);
			// 
			// button12
			// 
			this->button12->Location = System::Drawing::Point(8, 64);
			this->button12->Name = S"button12";
			this->button12->Size = System::Drawing::Size(96, 23);
			this->button12->TabIndex = 17;
			this->button12->Text = S"DISABLE";
			this->button12->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button12_Click);
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->button9);
			this->groupBox6->Controls->Add(this->button10);
			this->groupBox6->Location = System::Drawing::Point(120, 88);
			this->groupBox6->Name = S"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(112, 72);
			this->groupBox6->TabIndex = 24;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = S"ACE CMC ONE";
			// 
			// button9
			// 
			this->button9->Location = System::Drawing::Point(8, 16);
			this->button9->Name = S"button9";
			this->button9->Size = System::Drawing::Size(96, 23);
			this->button9->TabIndex = 16;
			this->button9->Text = S"ENABLE";
			this->button9->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button9_Click);
			// 
			// button10
			// 
			this->button10->Location = System::Drawing::Point(8, 40);
			this->button10->Name = S"button10";
			this->button10->Size = System::Drawing::Size(96, 23);
			this->button10->TabIndex = 17;
			this->button10->Text = S"DISABLE";
			this->button10->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button10_Click);
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->button7);
			this->groupBox5->Controls->Add(this->button8);
			this->groupBox5->Location = System::Drawing::Point(8, 88);
			this->groupBox5->Name = S"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(112, 72);
			this->groupBox5->TabIndex = 23;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = S"ACE CMC ZERO";
			// 
			// button7
			// 
			this->button7->Location = System::Drawing::Point(8, 16);
			this->button7->Name = S"button7";
			this->button7->Size = System::Drawing::Size(96, 23);
			this->button7->TabIndex = 16;
			this->button7->Text = S"ENABLE";
			this->button7->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button7_Click);
			// 
			// button8
			// 
			this->button8->Location = System::Drawing::Point(8, 40);
			this->button8->Name = S"button8";
			this->button8->Size = System::Drawing::Size(96, 23);
			this->button8->TabIndex = 17;
			this->button8->Text = S"DISABLE";
			this->button8->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button8_Click);
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->button5);
			this->groupBox4->Controls->Add(this->button6);
			this->groupBox4->Location = System::Drawing::Point(120, 16);
			this->groupBox4->Name = S"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(112, 72);
			this->groupBox4->TabIndex = 22;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = S"ABORT LT B";
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(8, 16);
			this->button5->Name = S"button5";
			this->button5->Size = System::Drawing::Size(96, 23);
			this->button5->TabIndex = 16;
			this->button5->Text = S"ON";
			this->button5->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button5_Click);
			// 
			// button6
			// 
			this->button6->Location = System::Drawing::Point(8, 40);
			this->button6->Name = S"button6";
			this->button6->Size = System::Drawing::Size(96, 23);
			this->button6->TabIndex = 17;
			this->button6->Text = S"OFF";
			this->button6->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button6_Click);
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->button3);
			this->groupBox3->Controls->Add(this->button4);
			this->groupBox3->Location = System::Drawing::Point(232, 16);
			this->groupBox3->Name = S"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(112, 72);
			this->groupBox3->TabIndex = 21;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = S"CREW ALARM";
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(8, 16);
			this->button3->Name = S"button3";
			this->button3->Size = System::Drawing::Size(96, 23);
			this->button3->TabIndex = 16;
			this->button3->Text = S"ON";
			this->button3->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button3_Click);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(8, 40);
			this->button4->Name = S"button4";
			this->button4->Size = System::Drawing::Size(96, 23);
			this->button4->TabIndex = 17;
			this->button4->Text = S"OFF";
			this->button4->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button4_Click);
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->Button1);
			this->groupBox2->Controls->Add(this->button2);
			this->groupBox2->Location = System::Drawing::Point(8, 16);
			this->groupBox2->Name = S"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(112, 72);
			this->groupBox2->TabIndex = 20;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = S"ABORT LT A";
			// 
			// Button1
			// 
			this->Button1->Location = System::Drawing::Point(8, 16);
			this->Button1->Name = S"Button1";
			this->Button1->Size = System::Drawing::Size(96, 23);
			this->Button1->TabIndex = 16;
			this->Button1->Text = S"ON";
			this->Button1->Click += new System::EventHandler(this, &GroundStation::UplinkForm::Button1_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(8, 40);
			this->button2->Name = S"button2";
			this->button2->Size = System::Drawing::Size(96, 23);
			this->button2->TabIndex = 17;
			this->button2->Text = S"OFF";
			this->button2->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button2_Click);
			// 
			// groupBox17
			// 
			this->groupBox17->Controls->Add(this->button43);
			this->groupBox17->Controls->Add(this->button44);
			this->groupBox17->Location = System::Drawing::Point(360, 8);
			this->groupBox17->Name = S"groupBox17";
			this->groupBox17->Size = System::Drawing::Size(112, 72);
			this->groupBox17->TabIndex = 22;
			this->groupBox17->TabStop = false;
			this->groupBox17->Text = S"UDL TEST";
			// 
			// button43
			// 
			this->button43->Location = System::Drawing::Point(8, 16);
			this->button43->Name = S"button43";
			this->button43->Size = System::Drawing::Size(96, 23);
			this->button43->TabIndex = 16;
			this->button43->Text = S"SEND \"A\"";
			// 
			// button44
			// 
			this->button44->Location = System::Drawing::Point(8, 40);
			this->button44->Name = S"button44";
			this->button44->Size = System::Drawing::Size(96, 23);
			this->button44->TabIndex = 17;
			this->button44->Text = S"SEND \"B\"";
			// 
			// groupBox18
			// 
			this->groupBox18->Controls->Add(this->button45);
			this->groupBox18->Controls->Add(this->textBox4);
			this->groupBox18->Controls->Add(this->label4);
			this->groupBox18->Controls->Add(this->textBox3);
			this->groupBox18->Controls->Add(this->label3);
			this->groupBox18->Controls->Add(this->textBox2);
			this->groupBox18->Controls->Add(this->label2);
			this->groupBox18->Controls->Add(this->textBox1);
			this->groupBox18->Controls->Add(this->label1);
			this->groupBox18->Location = System::Drawing::Point(472, 8);
			this->groupBox18->Name = S"groupBox18";
			this->groupBox18->Size = System::Drawing::Size(280, 72);
			this->groupBox18->TabIndex = 23;
			this->groupBox18->TabStop = false;
			this->groupBox18->Text = S"CTE RESET";
			// 
			// button45
			// 
			this->button45->Location = System::Drawing::Point(216, 16);
			this->button45->Name = S"button45";
			this->button45->Size = System::Drawing::Size(56, 48);
			this->button45->TabIndex = 17;
			this->button45->Text = S"SEND";
			// 
			// textBox4
			// 
			this->textBox4->Location = System::Drawing::Point(168, 40);
			this->textBox4->Name = S"textBox4";
			this->textBox4->Size = System::Drawing::Size(24, 20);
			this->textBox4->TabIndex = 7;
			this->textBox4->Text = S"59";
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(152, 16);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(64, 24);
			this->label4->TabIndex = 6;
			this->label4->Text = S"SECONDS";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// textBox3
			// 
			this->textBox3->Location = System::Drawing::Point(112, 40);
			this->textBox3->Name = S"textBox3";
			this->textBox3->Size = System::Drawing::Size(24, 20);
			this->textBox3->TabIndex = 5;
			this->textBox3->Text = S"59";
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(96, 16);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(56, 24);
			this->label3->TabIndex = 4;
			this->label3->Text = S"MINUTES";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(64, 40);
			this->textBox2->Name = S"textBox2";
			this->textBox2->Size = System::Drawing::Size(24, 20);
			this->textBox2->TabIndex = 3;
			this->textBox2->Text = S"23";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(48, 16);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(48, 24);
			this->label2->TabIndex = 2;
			this->label2->Text = S"HOURS";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(16, 40);
			this->textBox1->Name = S"textBox1";
			this->textBox1->Size = System::Drawing::Size(24, 20);
			this->textBox1->TabIndex = 1;
			this->textBox1->Text = S"19";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 16);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(40, 24);
			this->label1->TabIndex = 0;
			this->label1->Text = S"DAYS";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// groupBox19
			// 
			this->groupBox19->Controls->Add(this->groupBox21);
			this->groupBox19->Controls->Add(this->groupBox20);
			this->groupBox19->Location = System::Drawing::Point(360, 80);
			this->groupBox19->Name = S"groupBox19";
			this->groupBox19->Size = System::Drawing::Size(392, 400);
			this->groupBox19->TabIndex = 24;
			this->groupBox19->TabStop = false;
			this->groupBox19->Text = S"COMMAND MODULE COMPUTER";
			// 
			// groupBox21
			// 
			this->groupBox21->Controls->Add(this->button63);
			this->groupBox21->Controls->Add(this->button62);
			this->groupBox21->Controls->Add(this->button61);
			this->groupBox21->Controls->Add(this->button58);
			this->groupBox21->Controls->Add(this->button59);
			this->groupBox21->Controls->Add(this->button60);
			this->groupBox21->Controls->Add(this->button55);
			this->groupBox21->Controls->Add(this->button56);
			this->groupBox21->Controls->Add(this->button57);
			this->groupBox21->Controls->Add(this->button54);
			this->groupBox21->Controls->Add(this->button53);
			this->groupBox21->Controls->Add(this->button52);
			this->groupBox21->Controls->Add(this->button51);
			this->groupBox21->Controls->Add(this->button50);
			this->groupBox21->Controls->Add(this->button49);
			this->groupBox21->Controls->Add(this->button48);
			this->groupBox21->Controls->Add(this->button47);
			this->groupBox21->Controls->Add(this->button46);
			this->groupBox21->Controls->Add(this->label75);
			this->groupBox21->Controls->Add(this->label70);
			this->groupBox21->Controls->Add(this->label69);
			this->groupBox21->Controls->Add(this->textBox137);
			this->groupBox21->Controls->Add(this->label138);
			this->groupBox21->Controls->Add(this->textBox136);
			this->groupBox21->Controls->Add(this->label137);
			this->groupBox21->Controls->Add(this->textBox135);
			this->groupBox21->Controls->Add(this->label136);
			this->groupBox21->Controls->Add(this->textBox134);
			this->groupBox21->Controls->Add(this->label135);
			this->groupBox21->Controls->Add(this->textBox133);
			this->groupBox21->Controls->Add(this->label131);
			this->groupBox21->Controls->Add(this->textBox129);
			this->groupBox21->Controls->Add(this->label8);
			this->groupBox21->Controls->Add(this->label72);
			this->groupBox21->Location = System::Drawing::Point(8, 16);
			this->groupBox21->Name = S"groupBox21";
			this->groupBox21->Size = System::Drawing::Size(128, 376);
			this->groupBox21->TabIndex = 211;
			this->groupBox21->TabStop = false;
			this->groupBox21->Text = S"DSKY";
			// 
			// button63
			// 
			this->button63->Location = System::Drawing::Point(56, 344);
			this->button63->Name = S"button63";
			this->button63->Size = System::Drawing::Size(24, 23);
			this->button63->TabIndex = 197;
			this->button63->Text = S"-";
			this->button63->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button63_Click);
			// 
			// button62
			// 
			this->button62->Location = System::Drawing::Point(8, 344);
			this->button62->Name = S"button62";
			this->button62->Size = System::Drawing::Size(24, 23);
			this->button62->TabIndex = 196;
			this->button62->Text = S"+";
			this->button62->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button62_Click);
			// 
			// button61
			// 
			this->button61->Location = System::Drawing::Point(32, 344);
			this->button61->Name = S"button61";
			this->button61->Size = System::Drawing::Size(24, 23);
			this->button61->TabIndex = 195;
			this->button61->Text = S"0";
			this->button61->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button61_Click);
			// 
			// button58
			// 
			this->button58->Location = System::Drawing::Point(56, 320);
			this->button58->Name = S"button58";
			this->button58->Size = System::Drawing::Size(24, 23);
			this->button58->TabIndex = 194;
			this->button58->Text = S"9";
			this->button58->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button58_Click);
			// 
			// button59
			// 
			this->button59->Location = System::Drawing::Point(32, 320);
			this->button59->Name = S"button59";
			this->button59->Size = System::Drawing::Size(24, 23);
			this->button59->TabIndex = 193;
			this->button59->Text = S"8";
			this->button59->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button59_Click);
			// 
			// button60
			// 
			this->button60->Location = System::Drawing::Point(8, 320);
			this->button60->Name = S"button60";
			this->button60->Size = System::Drawing::Size(24, 23);
			this->button60->TabIndex = 192;
			this->button60->Text = S"7";
			this->button60->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button60_Click);
			// 
			// button55
			// 
			this->button55->Location = System::Drawing::Point(56, 296);
			this->button55->Name = S"button55";
			this->button55->Size = System::Drawing::Size(24, 23);
			this->button55->TabIndex = 191;
			this->button55->Text = S"6";
			this->button55->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button55_Click);
			// 
			// button56
			// 
			this->button56->Location = System::Drawing::Point(32, 296);
			this->button56->Name = S"button56";
			this->button56->Size = System::Drawing::Size(24, 23);
			this->button56->TabIndex = 190;
			this->button56->Text = S"5";
			this->button56->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button56_Click);
			// 
			// button57
			// 
			this->button57->Location = System::Drawing::Point(8, 296);
			this->button57->Name = S"button57";
			this->button57->Size = System::Drawing::Size(24, 23);
			this->button57->TabIndex = 189;
			this->button57->Text = S"4";
			this->button57->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button57_Click);
			// 
			// button54
			// 
			this->button54->Location = System::Drawing::Point(56, 272);
			this->button54->Name = S"button54";
			this->button54->Size = System::Drawing::Size(24, 23);
			this->button54->TabIndex = 188;
			this->button54->Text = S"3";
			this->button54->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button54_Click);
			// 
			// button53
			// 
			this->button53->Location = System::Drawing::Point(32, 272);
			this->button53->Name = S"button53";
			this->button53->Size = System::Drawing::Size(24, 23);
			this->button53->TabIndex = 187;
			this->button53->Text = S"2";
			this->button53->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button53_Click);
			// 
			// button52
			// 
			this->button52->Location = System::Drawing::Point(8, 272);
			this->button52->Name = S"button52";
			this->button52->Size = System::Drawing::Size(24, 23);
			this->button52->TabIndex = 186;
			this->button52->Text = S"1";
			this->button52->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button52_Click);
			// 
			// button51
			// 
			this->button51->Location = System::Drawing::Point(8, 240);
			this->button51->Name = S"button51";
			this->button51->Size = System::Drawing::Size(56, 23);
			this->button51->TabIndex = 185;
			this->button51->Text = S"K. REL";
			this->button51->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button51_Click);
			// 
			// button50
			// 
			this->button50->Location = System::Drawing::Point(64, 216);
			this->button50->Name = S"button50";
			this->button50->Size = System::Drawing::Size(56, 23);
			this->button50->TabIndex = 184;
			this->button50->Text = S"CLEAR";
			this->button50->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button50_Click);
			// 
			// button49
			// 
			this->button49->Location = System::Drawing::Point(8, 216);
			this->button49->Name = S"button49";
			this->button49->Size = System::Drawing::Size(56, 23);
			this->button49->TabIndex = 183;
			this->button49->Text = S"RESET";
			this->button49->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button49_Click);
			// 
			// button48
			// 
			this->button48->Location = System::Drawing::Point(64, 240);
			this->button48->Name = S"button48";
			this->button48->Size = System::Drawing::Size(56, 23);
			this->button48->TabIndex = 182;
			this->button48->Text = S"ENTER";
			this->button48->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button48_Click);
			// 
			// button47
			// 
			this->button47->Location = System::Drawing::Point(64, 192);
			this->button47->Name = S"button47";
			this->button47->Size = System::Drawing::Size(56, 23);
			this->button47->TabIndex = 181;
			this->button47->Text = S"NOUN";
			this->button47->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button47_Click);
			// 
			// button46
			// 
			this->button46->Location = System::Drawing::Point(8, 192);
			this->button46->Name = S"button46";
			this->button46->Size = System::Drawing::Size(56, 23);
			this->button46->TabIndex = 180;
			this->button46->Text = S"VERB";
			this->button46->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button46_Click);
			// 
			// label75
			// 
			this->label75->Location = System::Drawing::Point(56, 160);
			this->label75->Name = S"label75";
			this->label75->Size = System::Drawing::Size(64, 24);
			this->label75->TabIndex = 179;
			this->label75->Text = S"TRACKER";
			this->label75->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label70
			// 
			this->label70->Location = System::Drawing::Point(8, 160);
			this->label70->Name = S"label70";
			this->label70->Size = System::Drawing::Size(48, 24);
			this->label70->TabIndex = 173;
			this->label70->Text = S"GIMBAL LOCK";
			this->label70->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label69
			// 
			this->label69->Location = System::Drawing::Point(8, 136);
			this->label69->Name = S"label69";
			this->label69->Size = System::Drawing::Size(48, 24);
			this->label69->TabIndex = 172;
			this->label69->Text = S"NO ATT";
			this->label69->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox137
			// 
			this->textBox137->Enabled = false;
			this->textBox137->Location = System::Drawing::Point(72, 112);
			this->textBox137->Name = S"textBox137";
			this->textBox137->Size = System::Drawing::Size(48, 20);
			this->textBox137->TabIndex = 158;
			this->textBox137->Text = S"+00000";
			// 
			// label138
			// 
			this->label138->Location = System::Drawing::Point(8, 112);
			this->label138->Name = S"label138";
			this->label138->Size = System::Drawing::Size(40, 24);
			this->label138->TabIndex = 157;
			this->label138->Text = S"R3";
			this->label138->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox136
			// 
			this->textBox136->Enabled = false;
			this->textBox136->Location = System::Drawing::Point(72, 88);
			this->textBox136->Name = S"textBox136";
			this->textBox136->Size = System::Drawing::Size(48, 20);
			this->textBox136->TabIndex = 156;
			this->textBox136->Text = S"+00000";
			// 
			// label137
			// 
			this->label137->Location = System::Drawing::Point(8, 88);
			this->label137->Name = S"label137";
			this->label137->Size = System::Drawing::Size(40, 24);
			this->label137->TabIndex = 155;
			this->label137->Text = S"R2";
			this->label137->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox135
			// 
			this->textBox135->Enabled = false;
			this->textBox135->Location = System::Drawing::Point(72, 64);
			this->textBox135->Name = S"textBox135";
			this->textBox135->Size = System::Drawing::Size(48, 20);
			this->textBox135->TabIndex = 154;
			this->textBox135->Text = S"+00000";
			// 
			// label136
			// 
			this->label136->Location = System::Drawing::Point(8, 64);
			this->label136->Name = S"label136";
			this->label136->Size = System::Drawing::Size(40, 24);
			this->label136->TabIndex = 153;
			this->label136->Text = S"R1";
			this->label136->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox134
			// 
			this->textBox134->Enabled = false;
			this->textBox134->Location = System::Drawing::Point(96, 40);
			this->textBox134->Name = S"textBox134";
			this->textBox134->Size = System::Drawing::Size(24, 20);
			this->textBox134->TabIndex = 152;
			this->textBox134->Text = S"00";
			// 
			// label135
			// 
			this->label135->Location = System::Drawing::Point(72, 40);
			this->label135->Name = S"label135";
			this->label135->Size = System::Drawing::Size(16, 24);
			this->label135->TabIndex = 151;
			this->label135->Text = S"N";
			this->label135->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox133
			// 
			this->textBox133->Enabled = false;
			this->textBox133->Location = System::Drawing::Point(32, 40);
			this->textBox133->Name = S"textBox133";
			this->textBox133->Size = System::Drawing::Size(24, 20);
			this->textBox133->TabIndex = 150;
			this->textBox133->Text = S"00";
			// 
			// label131
			// 
			this->label131->Location = System::Drawing::Point(8, 40);
			this->label131->Name = S"label131";
			this->label131->Size = System::Drawing::Size(16, 24);
			this->label131->TabIndex = 149;
			this->label131->Text = S"V";
			this->label131->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox129
			// 
			this->textBox129->Enabled = false;
			this->textBox129->Location = System::Drawing::Point(96, 16);
			this->textBox129->Name = S"textBox129";
			this->textBox129->Size = System::Drawing::Size(24, 20);
			this->textBox129->TabIndex = 148;
			this->textBox129->Text = S"00";
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(8, 16);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(40, 24);
			this->label8->TabIndex = 147;
			this->label8->Text = S"PROG";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label72
			// 
			this->label72->ForeColor = System::Drawing::SystemColors::ControlText;
			this->label72->Location = System::Drawing::Point(56, 136);
			this->label72->Name = S"label72";
			this->label72->Size = System::Drawing::Size(56, 24);
			this->label72->TabIndex = 175;
			this->label72->Text = S"PROG";
			this->label72->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox20
			// 
			this->groupBox20->Controls->Add(this->textBox17);
			this->groupBox20->Controls->Add(this->label18);
			this->groupBox20->Controls->Add(this->button64);
			this->groupBox20->Controls->Add(this->textBox26);
			this->groupBox20->Controls->Add(this->label27);
			this->groupBox20->Controls->Add(this->textBox18);
			this->groupBox20->Controls->Add(this->label19);
			this->groupBox20->Controls->Add(this->textBox19);
			this->groupBox20->Controls->Add(this->label20);
			this->groupBox20->Controls->Add(this->textBox20);
			this->groupBox20->Controls->Add(this->label21);
			this->groupBox20->Controls->Add(this->textBox21);
			this->groupBox20->Controls->Add(this->label22);
			this->groupBox20->Controls->Add(this->textBox22);
			this->groupBox20->Controls->Add(this->label23);
			this->groupBox20->Controls->Add(this->textBox23);
			this->groupBox20->Controls->Add(this->label24);
			this->groupBox20->Controls->Add(this->textBox24);
			this->groupBox20->Controls->Add(this->label25);
			this->groupBox20->Controls->Add(this->textBox25);
			this->groupBox20->Controls->Add(this->label26);
			this->groupBox20->Controls->Add(this->textBox14);
			this->groupBox20->Controls->Add(this->label15);
			this->groupBox20->Controls->Add(this->textBox15);
			this->groupBox20->Controls->Add(this->label16);
			this->groupBox20->Controls->Add(this->textBox12);
			this->groupBox20->Controls->Add(this->label13);
			this->groupBox20->Controls->Add(this->textBox13);
			this->groupBox20->Controls->Add(this->label14);
			this->groupBox20->Controls->Add(this->textBox10);
			this->groupBox20->Controls->Add(this->label11);
			this->groupBox20->Controls->Add(this->textBox11);
			this->groupBox20->Controls->Add(this->label12);
			this->groupBox20->Controls->Add(this->textBox8);
			this->groupBox20->Controls->Add(this->label9);
			this->groupBox20->Controls->Add(this->textBox9);
			this->groupBox20->Controls->Add(this->label10);
			this->groupBox20->Controls->Add(this->comboBox1);
			this->groupBox20->Controls->Add(this->textBox7);
			this->groupBox20->Controls->Add(this->label7);
			this->groupBox20->Controls->Add(this->textBox6);
			this->groupBox20->Controls->Add(this->label6);
			this->groupBox20->Controls->Add(this->textBox5);
			this->groupBox20->Controls->Add(this->label5);
			this->groupBox20->Location = System::Drawing::Point(136, 16);
			this->groupBox20->Name = S"groupBox20";
			this->groupBox20->Size = System::Drawing::Size(248, 376);
			this->groupBox20->TabIndex = 2;
			this->groupBox20->TabStop = false;
			this->groupBox20->Text = S"P27 UPDATE";
			// 
			// textBox17
			// 
			this->textBox17->Enabled = false;
			this->textBox17->Location = System::Drawing::Point(72, 280);
			this->textBox17->Name = S"textBox17";
			this->textBox17->Size = System::Drawing::Size(48, 20);
			this->textBox17->TabIndex = 185;
			this->textBox17->Text = S"XXXXX";
			// 
			// label18
			// 
			this->label18->Location = System::Drawing::Point(8, 280);
			this->label18->Name = S"label18";
			this->label18->Size = System::Drawing::Size(64, 16);
			this->label18->TabIndex = 184;
			this->label18->Text = S"ADDR";
			this->label18->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// button64
			// 
			this->button64->Location = System::Drawing::Point(88, 336);
			this->button64->Name = S"button64";
			this->button64->Size = System::Drawing::Size(72, 23);
			this->button64->TabIndex = 183;
			this->button64->Text = S"TRANSMIT";
			this->button64->Click += new System::EventHandler(this, &GroundStation::UplinkForm::button64_Click);
			// 
			// textBox26
			// 
			this->textBox26->Enabled = false;
			this->textBox26->Location = System::Drawing::Point(72, 304);
			this->textBox26->Name = S"textBox26";
			this->textBox26->Size = System::Drawing::Size(168, 20);
			this->textBox26->TabIndex = 44;
			this->textBox26->Text = S"INACTIVE";
			// 
			// label27
			// 
			this->label27->Location = System::Drawing::Point(8, 304);
			this->label27->Name = S"label27";
			this->label27->Size = System::Drawing::Size(56, 16);
			this->label27->TabIndex = 43;
			this->label27->Text = S"STATUS";
			this->label27->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox18
			// 
			this->textBox18->Enabled = false;
			this->textBox18->Location = System::Drawing::Point(192, 256);
			this->textBox18->Name = S"textBox18";
			this->textBox18->Size = System::Drawing::Size(48, 20);
			this->textBox18->TabIndex = 38;
			this->textBox18->Text = S"XXXXX";
			// 
			// label19
			// 
			this->label19->Location = System::Drawing::Point(128, 256);
			this->label19->Name = S"label19";
			this->label19->Size = System::Drawing::Size(64, 16);
			this->label19->TabIndex = 37;
			this->label19->Text = S"DATA";
			this->label19->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox19
			// 
			this->textBox19->Enabled = false;
			this->textBox19->Location = System::Drawing::Point(72, 256);
			this->textBox19->Name = S"textBox19";
			this->textBox19->Size = System::Drawing::Size(48, 20);
			this->textBox19->TabIndex = 36;
			this->textBox19->Text = S"XXXXX";
			// 
			// label20
			// 
			this->label20->Location = System::Drawing::Point(8, 256);
			this->label20->Name = S"label20";
			this->label20->Size = System::Drawing::Size(64, 16);
			this->label20->TabIndex = 35;
			this->label20->Text = S"ADDR";
			this->label20->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox20
			// 
			this->textBox20->Enabled = false;
			this->textBox20->Location = System::Drawing::Point(192, 232);
			this->textBox20->Name = S"textBox20";
			this->textBox20->Size = System::Drawing::Size(48, 20);
			this->textBox20->TabIndex = 34;
			this->textBox20->Text = S"XXXXX";
			// 
			// label21
			// 
			this->label21->Location = System::Drawing::Point(128, 232);
			this->label21->Name = S"label21";
			this->label21->Size = System::Drawing::Size(64, 16);
			this->label21->TabIndex = 33;
			this->label21->Text = S"DATA";
			this->label21->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox21
			// 
			this->textBox21->Enabled = false;
			this->textBox21->Location = System::Drawing::Point(72, 232);
			this->textBox21->Name = S"textBox21";
			this->textBox21->Size = System::Drawing::Size(48, 20);
			this->textBox21->TabIndex = 32;
			this->textBox21->Text = S"XXXXX";
			// 
			// label22
			// 
			this->label22->Location = System::Drawing::Point(8, 232);
			this->label22->Name = S"label22";
			this->label22->Size = System::Drawing::Size(64, 16);
			this->label22->TabIndex = 31;
			this->label22->Text = S"ADDR";
			this->label22->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox22
			// 
			this->textBox22->Enabled = false;
			this->textBox22->Location = System::Drawing::Point(192, 208);
			this->textBox22->Name = S"textBox22";
			this->textBox22->Size = System::Drawing::Size(48, 20);
			this->textBox22->TabIndex = 30;
			this->textBox22->Text = S"XXXXX";
			// 
			// label23
			// 
			this->label23->Location = System::Drawing::Point(128, 208);
			this->label23->Name = S"label23";
			this->label23->Size = System::Drawing::Size(64, 16);
			this->label23->TabIndex = 29;
			this->label23->Text = S"DATA";
			this->label23->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox23
			// 
			this->textBox23->Enabled = false;
			this->textBox23->Location = System::Drawing::Point(72, 208);
			this->textBox23->Name = S"textBox23";
			this->textBox23->Size = System::Drawing::Size(48, 20);
			this->textBox23->TabIndex = 28;
			this->textBox23->Text = S"XXXXX";
			// 
			// label24
			// 
			this->label24->Location = System::Drawing::Point(8, 208);
			this->label24->Name = S"label24";
			this->label24->Size = System::Drawing::Size(64, 16);
			this->label24->TabIndex = 27;
			this->label24->Text = S"ADDR";
			this->label24->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox24
			// 
			this->textBox24->Enabled = false;
			this->textBox24->Location = System::Drawing::Point(192, 184);
			this->textBox24->Name = S"textBox24";
			this->textBox24->Size = System::Drawing::Size(48, 20);
			this->textBox24->TabIndex = 26;
			this->textBox24->Text = S"XXXXX";
			// 
			// label25
			// 
			this->label25->Location = System::Drawing::Point(128, 184);
			this->label25->Name = S"label25";
			this->label25->Size = System::Drawing::Size(64, 16);
			this->label25->TabIndex = 25;
			this->label25->Text = S"DATA";
			this->label25->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox25
			// 
			this->textBox25->Enabled = false;
			this->textBox25->Location = System::Drawing::Point(72, 184);
			this->textBox25->Name = S"textBox25";
			this->textBox25->Size = System::Drawing::Size(48, 20);
			this->textBox25->TabIndex = 24;
			this->textBox25->Text = S"XXXXX";
			// 
			// label26
			// 
			this->label26->Location = System::Drawing::Point(8, 184);
			this->label26->Name = S"label26";
			this->label26->Size = System::Drawing::Size(64, 16);
			this->label26->TabIndex = 23;
			this->label26->Text = S"ADDR";
			this->label26->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox14
			// 
			this->textBox14->Enabled = false;
			this->textBox14->Location = System::Drawing::Point(192, 160);
			this->textBox14->Name = S"textBox14";
			this->textBox14->Size = System::Drawing::Size(48, 20);
			this->textBox14->TabIndex = 22;
			this->textBox14->Text = S"XXXXX";
			// 
			// label15
			// 
			this->label15->Location = System::Drawing::Point(128, 160);
			this->label15->Name = S"label15";
			this->label15->Size = System::Drawing::Size(64, 16);
			this->label15->TabIndex = 21;
			this->label15->Text = S"DATA";
			this->label15->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox15
			// 
			this->textBox15->Enabled = false;
			this->textBox15->Location = System::Drawing::Point(72, 160);
			this->textBox15->Name = S"textBox15";
			this->textBox15->Size = System::Drawing::Size(48, 20);
			this->textBox15->TabIndex = 20;
			this->textBox15->Text = S"XXXXX";
			// 
			// label16
			// 
			this->label16->Location = System::Drawing::Point(8, 160);
			this->label16->Name = S"label16";
			this->label16->Size = System::Drawing::Size(64, 16);
			this->label16->TabIndex = 19;
			this->label16->Text = S"ADDR";
			this->label16->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox12
			// 
			this->textBox12->Enabled = false;
			this->textBox12->Location = System::Drawing::Point(192, 136);
			this->textBox12->Name = S"textBox12";
			this->textBox12->Size = System::Drawing::Size(48, 20);
			this->textBox12->TabIndex = 18;
			this->textBox12->Text = S"XXXXX";
			// 
			// label13
			// 
			this->label13->Location = System::Drawing::Point(128, 136);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(64, 16);
			this->label13->TabIndex = 17;
			this->label13->Text = S"DATA";
			this->label13->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox13
			// 
			this->textBox13->Enabled = false;
			this->textBox13->Location = System::Drawing::Point(72, 136);
			this->textBox13->Name = S"textBox13";
			this->textBox13->Size = System::Drawing::Size(48, 20);
			this->textBox13->TabIndex = 16;
			this->textBox13->Text = S"XXXXX";
			// 
			// label14
			// 
			this->label14->Location = System::Drawing::Point(8, 136);
			this->label14->Name = S"label14";
			this->label14->Size = System::Drawing::Size(64, 16);
			this->label14->TabIndex = 15;
			this->label14->Text = S"ADDR";
			this->label14->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox10
			// 
			this->textBox10->Enabled = false;
			this->textBox10->Location = System::Drawing::Point(192, 112);
			this->textBox10->Name = S"textBox10";
			this->textBox10->Size = System::Drawing::Size(48, 20);
			this->textBox10->TabIndex = 14;
			this->textBox10->Text = S"XXXXX";
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(128, 112);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(64, 16);
			this->label11->TabIndex = 13;
			this->label11->Text = S"DATA";
			this->label11->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox11
			// 
			this->textBox11->Enabled = false;
			this->textBox11->Location = System::Drawing::Point(72, 112);
			this->textBox11->Name = S"textBox11";
			this->textBox11->Size = System::Drawing::Size(48, 20);
			this->textBox11->TabIndex = 12;
			this->textBox11->Text = S"XXXXX";
			// 
			// label12
			// 
			this->label12->Location = System::Drawing::Point(8, 112);
			this->label12->Name = S"label12";
			this->label12->Size = System::Drawing::Size(64, 16);
			this->label12->TabIndex = 11;
			this->label12->Text = S"ADDR";
			this->label12->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox8
			// 
			this->textBox8->Enabled = false;
			this->textBox8->Location = System::Drawing::Point(192, 88);
			this->textBox8->Name = S"textBox8";
			this->textBox8->Size = System::Drawing::Size(48, 20);
			this->textBox8->TabIndex = 10;
			this->textBox8->Text = S"XXXXX";
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(128, 88);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(64, 16);
			this->label9->TabIndex = 9;
			this->label9->Text = S"DATA";
			this->label9->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox9
			// 
			this->textBox9->Enabled = false;
			this->textBox9->Location = System::Drawing::Point(72, 88);
			this->textBox9->Name = S"textBox9";
			this->textBox9->Size = System::Drawing::Size(48, 20);
			this->textBox9->TabIndex = 8;
			this->textBox9->Text = S"XXXXX";
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 88);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(64, 16);
			this->label10->TabIndex = 7;
			this->label10->Text = S"ADDR";
			this->label10->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// comboBox1
			// 
			this->comboBox1->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox1->Location = System::Drawing::Point(8, 16);
			this->comboBox1->Name = S"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(232, 21);
			this->comboBox1->TabIndex = 6;
			// 
			// textBox7
			// 
			this->textBox7->Enabled = false;
			this->textBox7->Location = System::Drawing::Point(192, 64);
			this->textBox7->Name = S"textBox7";
			this->textBox7->Size = System::Drawing::Size(48, 20);
			this->textBox7->TabIndex = 5;
			this->textBox7->Text = S"XXXXX";
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(128, 64);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(64, 16);
			this->label7->TabIndex = 4;
			this->label7->Text = S"DATA";
			this->label7->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox6
			// 
			this->textBox6->Enabled = false;
			this->textBox6->Location = System::Drawing::Point(72, 64);
			this->textBox6->Name = S"textBox6";
			this->textBox6->Size = System::Drawing::Size(48, 20);
			this->textBox6->TabIndex = 3;
			this->textBox6->Text = S"XXXXX";
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(8, 64);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(64, 16);
			this->label6->TabIndex = 2;
			this->label6->Text = S"ADDR";
			this->label6->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// textBox5
			// 
			this->textBox5->Enabled = false;
			this->textBox5->Location = System::Drawing::Point(72, 40);
			this->textBox5->Name = S"textBox5";
			this->textBox5->Size = System::Drawing::Size(48, 20);
			this->textBox5->TabIndex = 1;
			this->textBox5->Text = S"XXXXX";
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(8, 40);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(64, 16);
			this->label5->TabIndex = 0;
			this->label5->Text = S"INDEX";
			this->label5->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			// 
			// UplinkForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(760, 518);
			this->Controls->Add(this->groupBox19);
			this->Controls->Add(this->groupBox18);
			this->Controls->Add(this->groupBox17);
			this->Controls->Add(this->groupBox1);
			this->Name = S"UplinkForm";
			this->Text = S"DIGITAL UP-DATA LINK OPERATIONS";
			this->Load += new System::EventHandler(this, &GroundStation::UplinkForm::UplinkForm_Load);
			this->groupBox1->ResumeLayout(false);
			this->groupBox16->ResumeLayout(false);
			this->groupBox15->ResumeLayout(false);
			this->groupBox14->ResumeLayout(false);
			this->groupBox13->ResumeLayout(false);
			this->groupBox12->ResumeLayout(false);
			this->groupBox11->ResumeLayout(false);
			this->groupBox10->ResumeLayout(false);
			this->groupBox9->ResumeLayout(false);
			this->groupBox8->ResumeLayout(false);
			this->groupBox7->ResumeLayout(false);
			this->groupBox6->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->groupBox17->ResumeLayout(false);
			this->groupBox18->ResumeLayout(false);
			this->groupBox19->ResumeLayout(false);
			this->groupBox21->ResumeLayout(false);
			this->groupBox20->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
	
	private: System::Void button21_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				 int bytesXmit = SOCKET_ERROR;
				 char cmdbuf[4];
				 cmdbuf[0] = 046; // VA,SA
				 cmdbuf[1] = 043; // CMD
				 cmdbuf[2] = 046; // CMD 2
				 bytesXmit = send(*m_socket,cmdbuf,3,0);
			 }

private: System::Void UplinkForm_Load(System::Object *  sender, System::EventArgs *  e)
		 {
			 // Prepare P27 mode dropdown list
			 comboBox1->Items->Clear();
			 comboBox1->Items->Add(S"SELECT P27 UPDATE MODE");
			 comboBox1->Items->Add(S"V70 LIFTOFF TIME INC");
			 comboBox1->Items->Add(S"V71 CONTIGUOUS ERASABLE");
			 comboBox1->Items->Add(S"V71 A1501 STATE VECTOR");
			 comboBox1->Items->Add(S"V71 A0306 DESIRED REFSMMAT");
			 comboBox1->Items->Add(S"V71 A1735 CMC REFSMMAT");
			 comboBox1->Items->Add(S"V71 A3404 EXTERNAL DV");
			 comboBox1->Items->Add(S"V71 A3400 ENTRY TGT + EXT. DV");
			 comboBox1->Items->Add(S"V71 A3400 ENTRY TARGET");
			 comboBox1->Items->Add(S"V71 A3412 LAMBERT TARGET");
			 comboBox1->Items->Add(S"V71 A2025 LUNAR LANDING SITE");
			 comboBox1->Items->Add(S"V72 NON-CONTIGUOUS ERASABLE");
			 comboBox1->Items->Add(S"V73 OCTAL CLOCK INC");
			 comboBox1->SelectedIndex = 0;
			 // Setup handler
			 comboBox1->add_SelectedIndexChanged(new EventHandler(this, &GroundStation::UplinkForm::P27ModeChanged));
			 // Issue a reset
			 P27ModeChanged(NULL,NULL);
			 // Setup handler for INDEX
			 textBox5->add_TextChanged(new EventHandler(this, &GroundStation::UplinkForm::P27IndexChanged));
		 }

private: System::Void Button1_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 // Send ABORT LT A ON
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 001; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button2_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 000; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button4_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 004; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button3_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 005; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button6_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 006; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button5_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 007; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button8_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 010; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button7_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 011; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button10_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 014; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button9_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 015; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button13_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 022; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 026; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button11_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 022; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 027; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button12_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 023; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button14_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 032; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 036; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button15_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 032; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 037; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button16_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 033; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 037; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button17_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 040; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 050; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button18_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 040; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 051; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button19_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 041; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 044; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button20_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 041; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 045; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button22_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 042; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 046; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button23_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 042; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 047; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button24_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 043; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 047; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button25_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 052; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 056; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button26_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 052; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 057; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button27_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 053; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button29_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 062; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 066; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button30_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 062; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 067; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button31_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 063; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 066; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button28_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 063; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 067; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button32_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 064; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 070; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button33_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 064; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 071; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button34_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 065; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button36_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 072; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 076; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button37_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 072; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 077; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button38_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 073; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 076; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button35_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 073; // CMD
			 cmdbuf[2] = 046; // VA,SA
			 cmdbuf[3] = 077; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button39_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 074; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button40_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 046; // VA,SA
			 cmdbuf[1] = 075; // CMD
			 bytesXmit = send(*m_socket,cmdbuf,2,0);
		 }

private: System::Void button41_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 045; // VA,SA
			 cmdbuf[1] = 030; // CMD
			 cmdbuf[2] = 045; // VA,SA
			 cmdbuf[3] = 032; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button42_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 int bytesXmit = SOCKET_ERROR;
			 char cmdbuf[4];
			 cmdbuf[0] = 045; // VA,SA
			 cmdbuf[1] = 070; // CMD
			 cmdbuf[2] = 045; // VA,SA
			 cmdbuf[3] = 072; // CMD 2
			 bytesXmit = send(*m_socket,cmdbuf,4,0);
		 }

private: System::Void button46_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			send_agc_key('V');
		 }

private: System::Void button47_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('N');
		 }

private: System::Void button49_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('R');
		 }

private: System::Void button50_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('C');
		 }

private: System::Void button51_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('K');
		 }

private: System::Void button48_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('E');
		 }

private: System::Void button52_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('1');
		 }

private: System::Void button53_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('2');
		 }

private: System::Void button54_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('3');
		 }

private: System::Void button57_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('4');
		 }

private: System::Void button56_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('5');
		 }

private: System::Void button55_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('6');
		 }

private: System::Void button60_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('7');
		 }

private: System::Void button59_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('8');
		 }

private: System::Void button58_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('9');
		 }

private: System::Void button61_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('0');
		 }

private: System::Void button62_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('+');
		 }

private: System::Void button63_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 send_agc_key('-');
		 }

private: System::Void button64_Click(System::Object *  sender, System::EventArgs *  e)
		 {			 
			// Disable this button
			button64->Enabled = false;
			// Prevent CB changing
			comboBox1->Enabled = false;
			// Set up status
			textBox26->Enabled = true;
			textBox26->Text = "INITIALIZING";
			// And kick other thread
			cmc_uplink_state = 1;
		 }

};
}
