#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace GroundStation
{
	/// <summary> 
	/// Summary for ELSForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	__gc class ELSForm : public System::Windows::Forms::Form
	{
	public: 
		ELSForm(void)
		{
			InitializeComponent();
			this->add_Closing(new CancelEventHandler(this, &GroundStation::ELSForm::ELSForm_Closing));
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
	private: System::Void ELSForm_Closing(Object * sender, System::ComponentModel::CancelEventArgs* e){			
			e->Cancel = true; // don't close			
			this->Hide();     // Hide instead.
		}      		
	private: System::Windows::Forms::Label *  label19;
	private: System::Windows::Forms::TextBox *  s11A9;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::Label *  label6;
	private: System::Windows::Forms::Label *  label7;
	private: System::Windows::Forms::Label *  label8;
	private: System::Windows::Forms::Label *  label9;
	private: System::Windows::Forms::Label *  label10;
	private: System::Windows::Forms::Label *  label11;
	private: System::Windows::Forms::Label *  label12;
	private: System::Windows::Forms::Label *  label13;
	private: System::Windows::Forms::Label *  label14;
	private: System::Windows::Forms::Label *  label15;
	private: System::Windows::Forms::Label *  label16;
	private: System::Windows::Forms::Label *  label17;
	private: System::Windows::Forms::Label *  label18;
	private: System::Windows::Forms::Label *  label20;
	private: System::Windows::Forms::Label *  label21;




















	private: System::Windows::Forms::TextBox *  s11A5;
	private: System::Windows::Forms::TextBox *  s11E221;
	private: System::Windows::Forms::TextBox *  s11E231;
	private: System::Windows::Forms::TextBox *  s11E133;
	private: System::Windows::Forms::TextBox *  s11E136;
	private: System::Windows::Forms::TextBox *  s11E141;
	private: System::Windows::Forms::TextBox *  s11E134;
	private: System::Windows::Forms::TextBox *  s11E137;
	private: System::Windows::Forms::TextBox *  s11E144;
	private: System::Windows::Forms::TextBox *  s11E142;
	private: System::Windows::Forms::TextBox *  s11E233;
	private: System::Windows::Forms::TextBox *  s11E223;
	private: System::Windows::Forms::TextBox *  s11A8;
	private: System::Windows::Forms::TextBox *  s11A6;
	private: System::Windows::Forms::TextBox *  s11E227;
	private: System::Windows::Forms::TextBox *  s11E235;
	private: System::Windows::Forms::TextBox *  s11E157;
	private: System::Windows::Forms::TextBox *  s11E156;
	private: System::Windows::Forms::TextBox *  s11E265;
	private: System::Windows::Forms::TextBox *  s11E241;
	private: System::Windows::Forms::GroupBox *  groupBox1;
	private: System::Windows::Forms::GroupBox *  groupBox2;
	private: System::Windows::Forms::TextBox *  s11E291;
	private: System::Windows::Forms::Label *  label22;
	private: System::Windows::Forms::Label *  label23;
	private: System::Windows::Forms::Label *  label24;
	private: System::Windows::Forms::Label *  label25;
	private: System::Windows::Forms::Label *  label26;
	private: System::Windows::Forms::Label *  label27;
	private: System::Windows::Forms::TextBox *  s11E306;
	private: System::Windows::Forms::TextBox *  s11E298;
	private: System::Windows::Forms::TextBox *  s11E308;
	private: System::Windows::Forms::TextBox *  s11E295;
	private: System::Windows::Forms::TextBox *  s11E303;
	private: System::Windows::Forms::GroupBox *  groupBox3;
	private: System::Windows::Forms::Label *  label28;
	private: System::Windows::Forms::Label *  label29;
	private: System::Windows::Forms::Label *  label30;
	private: System::Windows::Forms::Label *  label31;
	private: System::Windows::Forms::Label *  label32;
	private: System::Windows::Forms::TextBox *  s11E45;
	private: System::Windows::Forms::TextBox *  s11E43;
	private: System::Windows::Forms::TextBox *  s11E46;
	private: System::Windows::Forms::TextBox *  s11A51;
	private: System::Windows::Forms::TextBox *  s10A22;





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
			this->label19 = new System::Windows::Forms::Label();
			this->s11A9 = new System::Windows::Forms::TextBox();
			this->label1 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->label3 = new System::Windows::Forms::Label();
			this->label4 = new System::Windows::Forms::Label();
			this->label5 = new System::Windows::Forms::Label();
			this->label6 = new System::Windows::Forms::Label();
			this->label7 = new System::Windows::Forms::Label();
			this->label8 = new System::Windows::Forms::Label();
			this->label9 = new System::Windows::Forms::Label();
			this->label10 = new System::Windows::Forms::Label();
			this->label11 = new System::Windows::Forms::Label();
			this->label12 = new System::Windows::Forms::Label();
			this->label13 = new System::Windows::Forms::Label();
			this->label14 = new System::Windows::Forms::Label();
			this->label15 = new System::Windows::Forms::Label();
			this->label16 = new System::Windows::Forms::Label();
			this->label17 = new System::Windows::Forms::Label();
			this->label18 = new System::Windows::Forms::Label();
			this->label20 = new System::Windows::Forms::Label();
			this->label21 = new System::Windows::Forms::Label();
			this->s11A5 = new System::Windows::Forms::TextBox();
			this->s11A8 = new System::Windows::Forms::TextBox();
			this->s11A6 = new System::Windows::Forms::TextBox();
			this->s11E221 = new System::Windows::Forms::TextBox();
			this->s11E231 = new System::Windows::Forms::TextBox();
			this->s11E133 = new System::Windows::Forms::TextBox();
			this->s11E136 = new System::Windows::Forms::TextBox();
			this->s11E141 = new System::Windows::Forms::TextBox();
			this->s11E134 = new System::Windows::Forms::TextBox();
			this->s11E137 = new System::Windows::Forms::TextBox();
			this->s11E227 = new System::Windows::Forms::TextBox();
			this->s11E144 = new System::Windows::Forms::TextBox();
			this->s11E142 = new System::Windows::Forms::TextBox();
			this->s11E233 = new System::Windows::Forms::TextBox();
			this->s11E223 = new System::Windows::Forms::TextBox();
			this->s11E235 = new System::Windows::Forms::TextBox();
			this->s11E157 = new System::Windows::Forms::TextBox();
			this->s11E156 = new System::Windows::Forms::TextBox();
			this->s11E265 = new System::Windows::Forms::TextBox();
			this->s11E241 = new System::Windows::Forms::TextBox();
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->groupBox2 = new System::Windows::Forms::GroupBox();
			this->s11E308 = new System::Windows::Forms::TextBox();
			this->s11E295 = new System::Windows::Forms::TextBox();
			this->s11E303 = new System::Windows::Forms::TextBox();
			this->s11E298 = new System::Windows::Forms::TextBox();
			this->s11E306 = new System::Windows::Forms::TextBox();
			this->label26 = new System::Windows::Forms::Label();
			this->label27 = new System::Windows::Forms::Label();
			this->label24 = new System::Windows::Forms::Label();
			this->label25 = new System::Windows::Forms::Label();
			this->label23 = new System::Windows::Forms::Label();
			this->s11E291 = new System::Windows::Forms::TextBox();
			this->label22 = new System::Windows::Forms::Label();
			this->groupBox3 = new System::Windows::Forms::GroupBox();
			this->label28 = new System::Windows::Forms::Label();
			this->label29 = new System::Windows::Forms::Label();
			this->label30 = new System::Windows::Forms::Label();
			this->label31 = new System::Windows::Forms::Label();
			this->label32 = new System::Windows::Forms::Label();
			this->s11E45 = new System::Windows::Forms::TextBox();
			this->s11E43 = new System::Windows::Forms::TextBox();
			this->s11E46 = new System::Windows::Forms::TextBox();
			this->s11A51 = new System::Windows::Forms::TextBox();
			this->s10A22 = new System::Windows::Forms::TextBox();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->SuspendLayout();
			// 
			// label19
			// 
			this->label19->Location = System::Drawing::Point(8, 16);
			this->label19->Name = S"label19";
			this->label19->Size = System::Drawing::Size(192, 24);
			this->label19->TabIndex = 39;
			this->label19->Text = S"PYRO BUS A VOLTS";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s11A9
			// 
			this->s11A9->Enabled = false;
			this->s11A9->Location = System::Drawing::Point(200, 16);
			this->s11A9->Name = S"s11A9";
			this->s11A9->Size = System::Drawing::Size(64, 20);
			this->s11A9->TabIndex = 42;
			this->s11A9->Text = S"XX.XX V";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 40);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(192, 24);
			this->label1->TabIndex = 43;
			this->label1->Text = S"PYRO BUS B VOLTS";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 64);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(192, 24);
			this->label2->TabIndex = 44;
			this->label2->Text = S"SLA SEP RELAY A";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 88);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(192, 24);
			this->label3->TabIndex = 45;
			this->label3->Text = S"SLA SEP RELAY B";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(8, 112);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(192, 24);
			this->label4->TabIndex = 46;
			this->label4->Text = S"CREW ABORT A";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(8, 136);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(192, 24);
			this->label5->TabIndex = 47;
			this->label5->Text = S"CREW ABORT B";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(8, 160);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(192, 24);
			this->label6->TabIndex = 48;
			this->label6->Text = S"EDS ABORT VOTE 1";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(8, 184);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(192, 24);
			this->label7->TabIndex = 49;
			this->label7->Text = S"EDS ABORT VOTE 2";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(8, 208);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(192, 24);
			this->label8->TabIndex = 50;
			this->label8->Text = S"EDS ABORT VOTE 3";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(8, 256);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(192, 24);
			this->label9->TabIndex = 52;
			this->label9->Text = S"EDS ABORT B";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 232);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(192, 24);
			this->label10->TabIndex = 51;
			this->label10->Text = S"EDS ABORT A";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(8, 280);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(192, 24);
			this->label11->TabIndex = 53;
			this->label11->Text = S"SCS CHANNEL ENABLE RCS A";
			this->label11->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label12
			// 
			this->label12->Location = System::Drawing::Point(8, 304);
			this->label12->Name = S"label12";
			this->label12->Size = System::Drawing::Size(192, 24);
			this->label12->TabIndex = 54;
			this->label12->Text = S"SCS CHANNEL ENABLE RCS B";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label13
			// 
			this->label13->Location = System::Drawing::Point(8, 328);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(192, 24);
			this->label13->TabIndex = 55;
			this->label13->Text = S"CM RCS PRESS SIG A";
			this->label13->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label14
			// 
			this->label14->Location = System::Drawing::Point(8, 352);
			this->label14->Name = S"label14";
			this->label14->Size = System::Drawing::Size(192, 24);
			this->label14->TabIndex = 56;
			this->label14->Text = S"CM RCS PRESS SIG B";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label15
			// 
			this->label15->Location = System::Drawing::Point(8, 376);
			this->label15->Name = S"label15";
			this->label15->Size = System::Drawing::Size(192, 24);
			this->label15->TabIndex = 57;
			this->label15->Text = S"LOGIC BUS A VOLTS";
			this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label16
			// 
			this->label16->Location = System::Drawing::Point(8, 400);
			this->label16->Name = S"label16";
			this->label16->Size = System::Drawing::Size(192, 24);
			this->label16->TabIndex = 58;
			this->label16->Text = S"LOGIC BUS B VOLTS";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label17
			// 
			this->label17->Location = System::Drawing::Point(8, 424);
			this->label17->Name = S"label17";
			this->label17->Size = System::Drawing::Size(192, 24);
			this->label17->TabIndex = 59;
			this->label17->Text = S"FWD HS JETT A";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label18
			// 
			this->label18->Location = System::Drawing::Point(8, 448);
			this->label18->Name = S"label18";
			this->label18->Size = System::Drawing::Size(192, 24);
			this->label18->TabIndex = 60;
			this->label18->Text = S"FWD HS JETT B";
			this->label18->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label20
			// 
			this->label20->Location = System::Drawing::Point(8, 472);
			this->label20->Name = S"label20";
			this->label20->Size = System::Drawing::Size(192, 24);
			this->label20->TabIndex = 61;
			this->label20->Text = S"CSM-LM LOCK RING SEP RELAY A";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label21
			// 
			this->label21->Location = System::Drawing::Point(8, 496);
			this->label21->Name = S"label21";
			this->label21->Size = System::Drawing::Size(192, 24);
			this->label21->TabIndex = 62;
			this->label21->Text = S"CSM-LM LOCK RING SEP RELAY B";
			this->label21->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s11A5
			// 
			this->s11A5->Enabled = false;
			this->s11A5->Location = System::Drawing::Point(200, 40);
			this->s11A5->Name = S"s11A5";
			this->s11A5->Size = System::Drawing::Size(64, 20);
			this->s11A5->TabIndex = 63;
			this->s11A5->Text = S"XX.XX V";
			// 
			// s11A8
			// 
			this->s11A8->Enabled = false;
			this->s11A8->Location = System::Drawing::Point(200, 376);
			this->s11A8->Name = S"s11A8";
			this->s11A8->Size = System::Drawing::Size(64, 20);
			this->s11A8->TabIndex = 64;
			this->s11A8->Text = S"XX.XX V";
			// 
			// s11A6
			// 
			this->s11A6->Enabled = false;
			this->s11A6->Location = System::Drawing::Point(200, 400);
			this->s11A6->Name = S"s11A6";
			this->s11A6->Size = System::Drawing::Size(64, 20);
			this->s11A6->TabIndex = 65;
			this->s11A6->Text = S"XX.XX V";
			// 
			// s11E221
			// 
			this->s11E221->Enabled = false;
			this->s11E221->Location = System::Drawing::Point(200, 64);
			this->s11E221->Name = S"s11E221";
			this->s11E221->Size = System::Drawing::Size(64, 20);
			this->s11E221->TabIndex = 101;
			this->s11E221->Text = S"XXXX";
			// 
			// s11E231
			// 
			this->s11E231->Enabled = false;
			this->s11E231->Location = System::Drawing::Point(200, 88);
			this->s11E231->Name = S"s11E231";
			this->s11E231->Size = System::Drawing::Size(64, 20);
			this->s11E231->TabIndex = 102;
			this->s11E231->Text = S"XXXX";
			// 
			// s11E133
			// 
			this->s11E133->Enabled = false;
			this->s11E133->Location = System::Drawing::Point(200, 112);
			this->s11E133->Name = S"s11E133";
			this->s11E133->Size = System::Drawing::Size(64, 20);
			this->s11E133->TabIndex = 103;
			this->s11E133->Text = S"XXXX";
			// 
			// s11E136
			// 
			this->s11E136->Enabled = false;
			this->s11E136->Location = System::Drawing::Point(200, 136);
			this->s11E136->Name = S"s11E136";
			this->s11E136->Size = System::Drawing::Size(64, 20);
			this->s11E136->TabIndex = 104;
			this->s11E136->Text = S"XXXX";
			// 
			// s11E141
			// 
			this->s11E141->Enabled = false;
			this->s11E141->Location = System::Drawing::Point(200, 160);
			this->s11E141->Name = S"s11E141";
			this->s11E141->Size = System::Drawing::Size(64, 20);
			this->s11E141->TabIndex = 105;
			this->s11E141->Text = S"XXXX";
			// 
			// s11E134
			// 
			this->s11E134->Enabled = false;
			this->s11E134->Location = System::Drawing::Point(200, 256);
			this->s11E134->Name = S"s11E134";
			this->s11E134->Size = System::Drawing::Size(64, 20);
			this->s11E134->TabIndex = 110;
			this->s11E134->Text = S"XXXX";
			// 
			// s11E137
			// 
			this->s11E137->Enabled = false;
			this->s11E137->Location = System::Drawing::Point(200, 232);
			this->s11E137->Name = S"s11E137";
			this->s11E137->Size = System::Drawing::Size(64, 20);
			this->s11E137->TabIndex = 109;
			this->s11E137->Text = S"XXXX";
			// 
			// s11E227
			// 
			this->s11E227->Enabled = false;
			this->s11E227->Location = System::Drawing::Point(200, 328);
			this->s11E227->Name = S"s11E227";
			this->s11E227->Size = System::Drawing::Size(64, 20);
			this->s11E227->TabIndex = 108;
			this->s11E227->Text = S"XXXX";
			// 
			// s11E144
			// 
			this->s11E144->Enabled = false;
			this->s11E144->Location = System::Drawing::Point(200, 208);
			this->s11E144->Name = S"s11E144";
			this->s11E144->Size = System::Drawing::Size(64, 20);
			this->s11E144->TabIndex = 107;
			this->s11E144->Text = S"XXXX";
			// 
			// s11E142
			// 
			this->s11E142->Enabled = false;
			this->s11E142->Location = System::Drawing::Point(200, 184);
			this->s11E142->Name = S"s11E142";
			this->s11E142->Size = System::Drawing::Size(64, 20);
			this->s11E142->TabIndex = 106;
			this->s11E142->Text = S"XXXX";
			// 
			// s11E233
			// 
			this->s11E233->Enabled = false;
			this->s11E233->Location = System::Drawing::Point(200, 304);
			this->s11E233->Name = S"s11E233";
			this->s11E233->Size = System::Drawing::Size(64, 20);
			this->s11E233->TabIndex = 112;
			this->s11E233->Text = S"XXXX";
			// 
			// s11E223
			// 
			this->s11E223->Enabled = false;
			this->s11E223->Location = System::Drawing::Point(200, 280);
			this->s11E223->Name = S"s11E223";
			this->s11E223->Size = System::Drawing::Size(64, 20);
			this->s11E223->TabIndex = 111;
			this->s11E223->Text = S"XXXX";
			// 
			// s11E235
			// 
			this->s11E235->Enabled = false;
			this->s11E235->Location = System::Drawing::Point(200, 352);
			this->s11E235->Name = S"s11E235";
			this->s11E235->Size = System::Drawing::Size(64, 20);
			this->s11E235->TabIndex = 113;
			this->s11E235->Text = S"XXXX";
			// 
			// s11E157
			// 
			this->s11E157->Enabled = false;
			this->s11E157->Location = System::Drawing::Point(200, 496);
			this->s11E157->Name = S"s11E157";
			this->s11E157->Size = System::Drawing::Size(64, 20);
			this->s11E157->TabIndex = 117;
			this->s11E157->Text = S"XXXX";
			// 
			// s11E156
			// 
			this->s11E156->Enabled = false;
			this->s11E156->Location = System::Drawing::Point(200, 472);
			this->s11E156->Name = S"s11E156";
			this->s11E156->Size = System::Drawing::Size(64, 20);
			this->s11E156->TabIndex = 116;
			this->s11E156->Text = S"XXXX";
			// 
			// s11E265
			// 
			this->s11E265->Enabled = false;
			this->s11E265->Location = System::Drawing::Point(200, 448);
			this->s11E265->Name = S"s11E265";
			this->s11E265->Size = System::Drawing::Size(64, 20);
			this->s11E265->TabIndex = 115;
			this->s11E265->Text = S"XXXX";
			// 
			// s11E241
			// 
			this->s11E241->Enabled = false;
			this->s11E241->Location = System::Drawing::Point(200, 424);
			this->s11E241->Name = S"s11E241";
			this->s11E241->Size = System::Drawing::Size(64, 20);
			this->s11E241->TabIndex = 114;
			this->s11E241->Text = S"XXXX";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label19);
			this->groupBox1->Controls->Add(this->label16);
			this->groupBox1->Controls->Add(this->s11A9);
			this->groupBox1->Controls->Add(this->label17);
			this->groupBox1->Controls->Add(this->label18);
			this->groupBox1->Controls->Add(this->label20);
			this->groupBox1->Controls->Add(this->label21);
			this->groupBox1->Controls->Add(this->s11A5);
			this->groupBox1->Controls->Add(this->s11A8);
			this->groupBox1->Controls->Add(this->s11A6);
			this->groupBox1->Controls->Add(this->s11E221);
			this->groupBox1->Controls->Add(this->s11E231);
			this->groupBox1->Controls->Add(this->s11E133);
			this->groupBox1->Controls->Add(this->s11E136);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->s11E141);
			this->groupBox1->Controls->Add(this->label8);
			this->groupBox1->Controls->Add(this->s11E134);
			this->groupBox1->Controls->Add(this->s11E137);
			this->groupBox1->Controls->Add(this->s11E227);
			this->groupBox1->Controls->Add(this->label15);
			this->groupBox1->Controls->Add(this->s11E144);
			this->groupBox1->Controls->Add(this->s11E142);
			this->groupBox1->Controls->Add(this->s11E233);
			this->groupBox1->Controls->Add(this->s11E223);
			this->groupBox1->Controls->Add(this->s11E235);
			this->groupBox1->Controls->Add(this->s11E157);
			this->groupBox1->Controls->Add(this->s11E156);
			this->groupBox1->Controls->Add(this->s11E265);
			this->groupBox1->Controls->Add(this->s11E241);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->label12);
			this->groupBox1->Controls->Add(this->label13);
			this->groupBox1->Controls->Add(this->label7);
			this->groupBox1->Controls->Add(this->label14);
			this->groupBox1->Controls->Add(this->label9);
			this->groupBox1->Controls->Add(this->label10);
			this->groupBox1->Controls->Add(this->label11);
			this->groupBox1->Location = System::Drawing::Point(8, 8);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(272, 528);
			this->groupBox1->TabIndex = 118;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"LAUNCH ESCAPE SYSTEM";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->s11E308);
			this->groupBox2->Controls->Add(this->s11E295);
			this->groupBox2->Controls->Add(this->s11E303);
			this->groupBox2->Controls->Add(this->s11E298);
			this->groupBox2->Controls->Add(this->s11E306);
			this->groupBox2->Controls->Add(this->label26);
			this->groupBox2->Controls->Add(this->label27);
			this->groupBox2->Controls->Add(this->label24);
			this->groupBox2->Controls->Add(this->label25);
			this->groupBox2->Controls->Add(this->label23);
			this->groupBox2->Controls->Add(this->s11E291);
			this->groupBox2->Controls->Add(this->label22);
			this->groupBox2->Location = System::Drawing::Point(288, 8);
			this->groupBox2->Name = S"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(272, 168);
			this->groupBox2->TabIndex = 119;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = S"EARTH LANDING SEQ CONTROL";
			// 
			// s11E308
			// 
			this->s11E308->Enabled = false;
			this->s11E308->Location = System::Drawing::Point(200, 136);
			this->s11E308->Name = S"s11E308";
			this->s11E308->Size = System::Drawing::Size(64, 20);
			this->s11E308->TabIndex = 113;
			this->s11E308->Text = S"XXXX";
			// 
			// s11E295
			// 
			this->s11E295->Enabled = false;
			this->s11E295->Location = System::Drawing::Point(200, 112);
			this->s11E295->Name = S"s11E295";
			this->s11E295->Size = System::Drawing::Size(64, 20);
			this->s11E295->TabIndex = 112;
			this->s11E295->Text = S"XXXX";
			// 
			// s11E303
			// 
			this->s11E303->Enabled = false;
			this->s11E303->Location = System::Drawing::Point(200, 88);
			this->s11E303->Name = S"s11E303";
			this->s11E303->Size = System::Drawing::Size(64, 20);
			this->s11E303->TabIndex = 111;
			this->s11E303->Text = S"XXXX";
			// 
			// s11E298
			// 
			this->s11E298->Enabled = false;
			this->s11E298->Location = System::Drawing::Point(200, 64);
			this->s11E298->Name = S"s11E298";
			this->s11E298->Size = System::Drawing::Size(64, 20);
			this->s11E298->TabIndex = 110;
			this->s11E298->Text = S"XXXX";
			// 
			// s11E306
			// 
			this->s11E306->Enabled = false;
			this->s11E306->Location = System::Drawing::Point(200, 40);
			this->s11E306->Name = S"s11E306";
			this->s11E306->Size = System::Drawing::Size(64, 20);
			this->s11E306->TabIndex = 109;
			this->s11E306->Text = S"XXXX";
			// 
			// label26
			// 
			this->label26->Location = System::Drawing::Point(8, 136);
			this->label26->Name = S"label26";
			this->label26->Size = System::Drawing::Size(192, 24);
			this->label26->TabIndex = 108;
			this->label26->Text = S"MAIN CHUTE DISC RELAY B";
			this->label26->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label27
			// 
			this->label27->Location = System::Drawing::Point(8, 112);
			this->label27->Name = S"label27";
			this->label27->Size = System::Drawing::Size(192, 24);
			this->label27->TabIndex = 107;
			this->label27->Text = S"MAIN CHUTE DISC RELAY A";
			this->label27->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label24
			// 
			this->label24->Location = System::Drawing::Point(8, 88);
			this->label24->Name = S"label24";
			this->label24->Size = System::Drawing::Size(192, 24);
			this->label24->TabIndex = 106;
			this->label24->Text = S"MAIN DEP RELAY B";
			this->label24->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label25
			// 
			this->label25->Location = System::Drawing::Point(8, 64);
			this->label25->Name = S"label25";
			this->label25->Size = System::Drawing::Size(192, 24);
			this->label25->TabIndex = 105;
			this->label25->Text = S"MAIN DEP RELAY A";
			this->label25->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label23
			// 
			this->label23->Location = System::Drawing::Point(8, 40);
			this->label23->Name = S"label23";
			this->label23->Size = System::Drawing::Size(192, 24);
			this->label23->TabIndex = 104;
			this->label23->Text = S"DROGUE DEP RELAY B";
			this->label23->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s11E291
			// 
			this->s11E291->Enabled = false;
			this->s11E291->Location = System::Drawing::Point(200, 16);
			this->s11E291->Name = S"s11E291";
			this->s11E291->Size = System::Drawing::Size(64, 20);
			this->s11E291->TabIndex = 103;
			this->s11E291->Text = S"XXXX";
			// 
			// label22
			// 
			this->label22->Location = System::Drawing::Point(8, 16);
			this->label22->Name = S"label22";
			this->label22->Size = System::Drawing::Size(192, 24);
			this->label22->TabIndex = 102;
			this->label22->Text = S"DROGUE DEP RELAY A";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->s10A22);
			this->groupBox3->Controls->Add(this->s11A51);
			this->groupBox3->Controls->Add(this->s11E46);
			this->groupBox3->Controls->Add(this->s11E43);
			this->groupBox3->Controls->Add(this->s11E45);
			this->groupBox3->Controls->Add(this->label32);
			this->groupBox3->Controls->Add(this->label31);
			this->groupBox3->Controls->Add(this->label30);
			this->groupBox3->Controls->Add(this->label29);
			this->groupBox3->Controls->Add(this->label28);
			this->groupBox3->Location = System::Drawing::Point(288, 184);
			this->groupBox3->Name = S"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(272, 144);
			this->groupBox3->TabIndex = 120;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = S"CREW SAFETY SYSTEM";
			// 
			// label28
			// 
			this->label28->Location = System::Drawing::Point(8, 16);
			this->label28->Name = S"label28";
			this->label28->Size = System::Drawing::Size(192, 24);
			this->label28->TabIndex = 108;
			this->label28->Text = S"SM EDS ABORT REQUEST A";
			this->label28->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label29
			// 
			this->label29->Location = System::Drawing::Point(8, 40);
			this->label29->Name = S"label29";
			this->label29->Size = System::Drawing::Size(192, 24);
			this->label29->TabIndex = 109;
			this->label29->Text = S"SM EDS ABORT REQUEST B";
			this->label29->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label30
			// 
			this->label30->Location = System::Drawing::Point(8, 64);
			this->label30->Name = S"label30";
			this->label30->Size = System::Drawing::Size(192, 24);
			this->label30->TabIndex = 110;
			this->label30->Text = S"MASTER CAUTION WARNING ON";
			this->label30->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label31
			// 
			this->label31->Location = System::Drawing::Point(8, 88);
			this->label31->Name = S"label31";
			this->label31->Size = System::Drawing::Size(192, 24);
			this->label31->TabIndex = 111;
			this->label31->Text = S"ANGLE OF ATTACK";
			this->label31->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label32
			// 
			this->label32->Location = System::Drawing::Point(8, 112);
			this->label32->Name = S"label32";
			this->label32->Size = System::Drawing::Size(192, 24);
			this->label32->TabIndex = 112;
			this->label32->Text = S"DOCKING PROBE TEMP";
			this->label32->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s11E45
			// 
			this->s11E45->Enabled = false;
			this->s11E45->Location = System::Drawing::Point(200, 16);
			this->s11E45->Name = S"s11E45";
			this->s11E45->Size = System::Drawing::Size(64, 20);
			this->s11E45->TabIndex = 113;
			this->s11E45->Text = S"XXXX";
			// 
			// s11E43
			// 
			this->s11E43->Enabled = false;
			this->s11E43->Location = System::Drawing::Point(200, 40);
			this->s11E43->Name = S"s11E43";
			this->s11E43->Size = System::Drawing::Size(64, 20);
			this->s11E43->TabIndex = 114;
			this->s11E43->Text = S"XXXX";
			// 
			// s11E46
			// 
			this->s11E46->Enabled = false;
			this->s11E46->Location = System::Drawing::Point(200, 64);
			this->s11E46->Name = S"s11E46";
			this->s11E46->Size = System::Drawing::Size(64, 20);
			this->s11E46->TabIndex = 115;
			this->s11E46->Text = S"XXXX";
			// 
			// s11A51
			// 
			this->s11A51->Enabled = false;
			this->s11A51->Location = System::Drawing::Point(200, 88);
			this->s11A51->Name = S"s11A51";
			this->s11A51->Size = System::Drawing::Size(64, 20);
			this->s11A51->TabIndex = 116;
			this->s11A51->Text = S"XX.XX V";
			// 
			// s10A22
			// 
			this->s10A22->Enabled = false;
			this->s10A22->Location = System::Drawing::Point(200, 112);
			this->s10A22->Name = S"s10A22";
			this->s10A22->Size = System::Drawing::Size(64, 20);
			this->s10A22->TabIndex = 117;
			this->s10A22->Text = S"XXX.XX °F";
			// 
			// ELSForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(566, 540);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = S"ELSForm";
			this->Text = S"LES / ELS / CSS";
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
};
}