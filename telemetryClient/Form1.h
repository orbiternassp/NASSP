#pragma once

/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2008

  Telemetry Client main window

  Project Apollo is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Project Apollo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Project Apollo; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  See http://nassp.sourceforge.net/license/ for more details.

  **************************************************************************/

namespace GroundStation
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;

	// Winsock
	WSADATA wsaData;
	SOCKET m_socket;
	sockaddr_in clientService;
	int conn_status;

	/// <summary> 
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class Form1 : public System::Windows::Forms::Form
	{	
	public:
		Form1(void)
		{
			InitializeComponent();
		}
  
	protected:
		void Dispose(Boolean disposing)
		{
			if (trd != NULL && trd->IsAlive)
				trd->Abort();
			WSACleanup();
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::Button *  ConnectBtn;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::TextBox *  HostAddrBox;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::TextBox *  StatusBox;
	private: System::Windows::Forms::Button *  ShowStrBtn;
	private: System::Windows::Forms::Button *  ShowEPSBtn;
	private: System::Windows::Forms::Button *  ShowLESBtn;
	private: System::Windows::Forms::Button *  ShowECSBtn;
	private: System::Windows::Forms::Button *  ShowGNBtn;
	private: System::Windows::Forms::Button *  ShowSCSBtn;
	private: System::Windows::Forms::Button *  ShowCrewBtn;
	private: System::Windows::Forms::Button *  ShowSPSBtn;
	private: System::Windows::Forms::Button *  ShowTCMBtn;
	private: System::Windows::Forms::GroupBox *  groupBox1;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container * components;
	private: System::Windows::Forms::Button *  ShowCMCBtn;
	private: System::Windows::Forms::Button *  ShowUplinkBtn;

		// Crap
		// Service Thread
		Thread *trd;
		void CommThread();
		// Functions
		void WinsockInit();
		void ConnectToHost();
		void ShowStructures();
		void ShowEPS();
		void ShowELS();
		void ShowECS();
		void ShowGNC();
		void ShowSCS();
		void ShowSPS();
		void ShowCrew();
		void ShowTelecom();
		void ShowCMC();
		void ShowUplink();
		void cmc_uplink_process();
		void cmc_uplink_word(char * data);
		int lock_type;
		int framect;
		int framead;
		int frame_addr;
		// CMC datastream parser
		unsigned int cmc_w0,cmc_w1;
		unsigned int cmc_upbuff __nogc[20];
		unsigned int cmc_compnumb;
		unsigned int cmc_upoldmod;
		unsigned int cmc_upverb;
		unsigned int cmc_upcount;
		unsigned int cmc_upindex;
		int cmc_lock_type;
		int cmc_framect;
		int cmc_frame_addr;
		unsigned int dsptab __nogc[12]; // DSKY Display Table
		// Other forms
		__gc class StructuresForm * str_form;
		__gc class ELSForm *        els_form;
		__gc class ECSForm *        ecs_form;
		__gc class GNCForm *        gnc_form;
		__gc class SCSForm *        scs_form;
		__gc class SPSForm *        sps_form;
		__gc class CrewForm *       crw_form;
		__gc class TelecomForm *    tcm_form;
		__gc class EPSForm *        eps_form;
		__gc class CMCForm *        cmc_form;
		__gc class UplinkForm *     upl_form;
		// Other utility
		void end_hbr();
		void end_lbr();
		void parse_cmc();
		void setup_cmc_list();
		char get_dsky_char(unsigned int bits);
		double unscale_data(unsigned char data,double low,double high);
		void parse_hbr(unsigned char data, int bytect);
		void parse_lbr(unsigned char data, int bytect);
		void display(unsigned char data, int channel, int type, int ccode);

		typedef System::Windows::Forms::TextBox textDisplay;

		void showValue( textDisplay *tb, char *msg );
		void showPercentage( textDisplay *tb, unsigned char data );
		void showPercentage( textDisplay *tb, unsigned char data, double maxPercent );
		void showSci( textDisplay *tb, unsigned char data );
		void showPSIA( textDisplay *tb, unsigned char data, double low, double high );
		void showTempF( textDisplay *tb, unsigned char data, double low, double high );

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->ConnectBtn = new System::Windows::Forms::Button();
			this->label1 = new System::Windows::Forms::Label();
			this->HostAddrBox = new System::Windows::Forms::TextBox();
			this->label2 = new System::Windows::Forms::Label();
			this->StatusBox = new System::Windows::Forms::TextBox();
			this->ShowStrBtn = new System::Windows::Forms::Button();
			this->ShowEPSBtn = new System::Windows::Forms::Button();
			this->ShowLESBtn = new System::Windows::Forms::Button();
			this->ShowECSBtn = new System::Windows::Forms::Button();
			this->ShowGNBtn = new System::Windows::Forms::Button();
			this->ShowSCSBtn = new System::Windows::Forms::Button();
			this->ShowCrewBtn = new System::Windows::Forms::Button();
			this->ShowSPSBtn = new System::Windows::Forms::Button();
			this->ShowTCMBtn = new System::Windows::Forms::Button();
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->ShowCMCBtn = new System::Windows::Forms::Button();
			this->ShowUplinkBtn = new System::Windows::Forms::Button();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// ConnectBtn
			// 
			this->ConnectBtn->Location = System::Drawing::Point(200, 8);
			this->ConnectBtn->Name = S"ConnectBtn";
			this->ConnectBtn->TabIndex = 0;
			this->ConnectBtn->Text = S"Connect";
			this->ConnectBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ConnectBtn_Click);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 8);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(80, 24);
			this->label1->TabIndex = 1;
			this->label1->Text = S"Host Address:";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// HostAddrBox
			// 
			this->HostAddrBox->Location = System::Drawing::Point(88, 8);
			this->HostAddrBox->Name = S"HostAddrBox";
			this->HostAddrBox->Size = System::Drawing::Size(104, 20);
			this->HostAddrBox->TabIndex = 2;
			this->HostAddrBox->Text = S"127.0.0.1";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 40);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(40, 24);
			this->label2->TabIndex = 3;
			this->label2->Text = S"Status:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// StatusBox
			// 
			this->StatusBox->Enabled = false;
			this->StatusBox->Location = System::Drawing::Point(48, 40);
			this->StatusBox->Name = S"StatusBox";
			this->StatusBox->Size = System::Drawing::Size(224, 20);
			this->StatusBox->TabIndex = 4;
			this->StatusBox->Text = S"";
			// 
			// ShowStrBtn
			// 
			this->ShowStrBtn->Location = System::Drawing::Point(176, 16);
			this->ShowStrBtn->Name = S"ShowStrBtn";
			this->ShowStrBtn->TabIndex = 5;
			this->ShowStrBtn->Text = S"STRUCT";
			this->ShowStrBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ShowStrBtn_Click);
			// 
			// ShowEPSBtn
			// 
			this->ShowEPSBtn->Location = System::Drawing::Point(96, 64);
			this->ShowEPSBtn->Name = S"ShowEPSBtn";
			this->ShowEPSBtn->TabIndex = 6;
			this->ShowEPSBtn->Text = S"EPS";
			this->ShowEPSBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ShowEPSBtn_Click);
			// 
			// ShowLESBtn
			// 
			this->ShowLESBtn->Location = System::Drawing::Point(16, 16);
			this->ShowLESBtn->Name = S"ShowLESBtn";
			this->ShowLESBtn->TabIndex = 7;
			this->ShowLESBtn->Text = S"LES/ELS";
			this->ShowLESBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ShowLESBtn_Click);
			// 
			// ShowECSBtn
			// 
			this->ShowECSBtn->Location = System::Drawing::Point(16, 40);
			this->ShowECSBtn->Name = S"ShowECSBtn";
			this->ShowECSBtn->TabIndex = 8;
			this->ShowECSBtn->Text = S"ECS";
			this->ShowECSBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ShowECSBtn_Click);
			// 
			// ShowGNBtn
			// 
			this->ShowGNBtn->Location = System::Drawing::Point(96, 16);
			this->ShowGNBtn->Name = S"ShowGNBtn";
			this->ShowGNBtn->TabIndex = 9;
			this->ShowGNBtn->Text = S"GNC/FLT";
			this->ShowGNBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ShowGNBtn_Click);
			// 
			// ShowSCSBtn
			// 
			this->ShowSCSBtn->Location = System::Drawing::Point(96, 40);
			this->ShowSCSBtn->Name = S"ShowSCSBtn";
			this->ShowSCSBtn->TabIndex = 10;
			this->ShowSCSBtn->Text = S"SCS";
			this->ShowSCSBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ShowSCSBtn_Click);
			// 
			// ShowCrewBtn
			// 
			this->ShowCrewBtn->Location = System::Drawing::Point(16, 64);
			this->ShowCrewBtn->Name = S"ShowCrewBtn";
			this->ShowCrewBtn->TabIndex = 11;
			this->ShowCrewBtn->Text = S"CREW/SCI";
			this->ShowCrewBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ShowCrewBtn_Click);
			// 
			// ShowSPSBtn
			// 
			this->ShowSPSBtn->Location = System::Drawing::Point(176, 40);
			this->ShowSPSBtn->Name = S"ShowSPSBtn";
			this->ShowSPSBtn->TabIndex = 12;
			this->ShowSPSBtn->Text = S"SPS/RCS";
			this->ShowSPSBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ShowSPSBtn_Click);
			// 
			// ShowTCMBtn
			// 
			this->ShowTCMBtn->Location = System::Drawing::Point(176, 64);
			this->ShowTCMBtn->Name = S"ShowTCMBtn";
			this->ShowTCMBtn->TabIndex = 13;
			this->ShowTCMBtn->Text = S"TELECOM";
			this->ShowTCMBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ShowTCMBtn_Click);
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->ShowCMCBtn);
			this->groupBox1->Controls->Add(this->ShowCrewBtn);
			this->groupBox1->Controls->Add(this->ShowSCSBtn);
			this->groupBox1->Controls->Add(this->ShowTCMBtn);
			this->groupBox1->Controls->Add(this->ShowLESBtn);
			this->groupBox1->Controls->Add(this->ShowSPSBtn);
			this->groupBox1->Controls->Add(this->ShowECSBtn);
			this->groupBox1->Controls->Add(this->ShowGNBtn);
			this->groupBox1->Controls->Add(this->ShowEPSBtn);
			this->groupBox1->Controls->Add(this->ShowStrBtn);
			this->groupBox1->Location = System::Drawing::Point(8, 144);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(264, 120);
			this->groupBox1->TabIndex = 14;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"Downlink Data Display Select";
			// 
			// ShowCMCBtn
			// 
			this->ShowCMCBtn->Location = System::Drawing::Point(96, 88);
			this->ShowCMCBtn->Name = S"ShowCMCBtn";
			this->ShowCMCBtn->TabIndex = 113;
			this->ShowCMCBtn->Text = S"CMC DATA";
			this->ShowCMCBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ShowCMCBtn_Click);
			// 
			// ShowUplinkBtn
			// 
			this->ShowUplinkBtn->Location = System::Drawing::Point(104, 112);
			this->ShowUplinkBtn->Name = S"ShowUplinkBtn";
			this->ShowUplinkBtn->TabIndex = 15;
			this->ShowUplinkBtn->Text = S"UPLINK";
			this->ShowUplinkBtn->Click += new System::EventHandler(this, &GroundStation::Form1::ShowUplinkBtn_Click);
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(278, 266);
			this->Controls->Add(this->ShowUplinkBtn);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->StatusBox);
			this->Controls->Add(this->HostAddrBox);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->ConnectBtn);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = S"Form1";
			this->Text = S"Apollo Ground Station";
			this->Load += new System::EventHandler(this, &GroundStation::Form1::Form1_Load);
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);

		}	
	private: System::Void ConnectBtn_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				 ConnectToHost();
			 }

	private: System::Void Form1_Load(System::Object *  sender, System::EventArgs *  e)
			 {
				 // Setup
				 trd = NULL;
				 WinsockInit();				 
			 }
	private: System::Void ShowStrBtn_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				ShowStructures();
			}

private: System::Void ShowEPSBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 ShowEPS();
		 }

private: System::Void ShowLESBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 ShowELS();
		 }

private: System::Void ShowECSBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 ShowECS();
		 }

private: System::Void ShowGNBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 ShowGNC();
		 }

private: System::Void ShowSCSBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 ShowSCS();
		 }

private: System::Void ShowCrewBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 ShowCrew();
		 }

private: System::Void ShowSPSBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			ShowSPS();
		 }

private: System::Void ShowTCMBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			ShowTelecom();
		 }

private: System::Void ShowCMCBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			ShowCMC();
		 }

private: System::Void ShowUplinkBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			ShowUplink();
		 }

private: void ThreadTask()
		{
			CommThread();
		}


};
}


