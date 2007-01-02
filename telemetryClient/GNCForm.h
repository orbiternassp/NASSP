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
	/// Summary for GNCForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	__gc class GNCForm : public System::Windows::Forms::Form
	{
	public: 
		GNCForm(void)
		{
			InitializeComponent();
			this->add_Closing(new CancelEventHandler(this,GNCForm_Closing));
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
	private: System::Void GNCForm_Closing(Object * sender, System::ComponentModel::CancelEventArgs* e){			
			e->Cancel = true; // don't close			
			this->Hide();     // Hide instead.
		}      		
	private: System::Windows::Forms::GroupBox *  groupBox1;
	private: System::Windows::Forms::TextBox *  s11E191;
	private: System::Windows::Forms::TextBox *  s12A3;
	private: System::Windows::Forms::TextBox *  s12A1;
	private: System::Windows::Forms::TextBox *  s10A85;
	private: System::Windows::Forms::TextBox *  s11E154;
	private: System::Windows::Forms::TextBox *  s11E153;
	private: System::Windows::Forms::TextBox *  s11E152;
	private: System::Windows::Forms::TextBox *  s12A2;
	private: System::Windows::Forms::TextBox *  s10A96;
	private: System::Windows::Forms::TextBox *  s11A63;
	private: System::Windows::Forms::TextBox *  s11A62;
	private: System::Windows::Forms::TextBox *  s11A61;
	private: System::Windows::Forms::TextBox *  s11A60;
	private: System::Windows::Forms::TextBox *  s11A59;
	private: System::Windows::Forms::TextBox *  s11A54;
	private: System::Windows::Forms::TextBox *  s11A53;
	private: System::Windows::Forms::TextBox *  s11A52;
	private: System::Windows::Forms::TextBox *  s10A91;
	private: System::Windows::Forms::TextBox *  s10A138;
	private: System::Windows::Forms::Label *  label20;
	private: System::Windows::Forms::Label *  label19;
	private: System::Windows::Forms::Label *  label18;
	private: System::Windows::Forms::Label *  label17;
	private: System::Windows::Forms::Label *  label14;
	private: System::Windows::Forms::Label *  label15;
	private: System::Windows::Forms::Label *  label16;
	private: System::Windows::Forms::Label *  label11;
	private: System::Windows::Forms::Label *  label12;
	private: System::Windows::Forms::Label *  label13;
	private: System::Windows::Forms::Label *  label10;
	private: System::Windows::Forms::Label *  label9;
	private: System::Windows::Forms::Label *  label8;
	private: System::Windows::Forms::Label *  label7;
	private: System::Windows::Forms::Label *  label6;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::TextBox *  s10A83;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::GroupBox *  groupBox2;
	private: System::Windows::Forms::Label *  label21;
	private: System::Windows::Forms::Label *  label22;
	private: System::Windows::Forms::Label *  label23;
	private: System::Windows::Forms::TextBox *  s12A9;
	private: System::Windows::Forms::TextBox *  s12A11;
	private: System::Windows::Forms::TextBox *  s12A12;
	private: System::Windows::Forms::Label *  label24;
	private: System::Windows::Forms::Label *  label25;
	private: System::Windows::Forms::Label *  label26;
	private: System::Windows::Forms::TextBox *  s11A169;
	private: System::Windows::Forms::TextBox *  s11A16;
	private: System::Windows::Forms::TextBox *  s10A149;






















































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
			this->s11E191 = new System::Windows::Forms::TextBox();
			this->s12A3 = new System::Windows::Forms::TextBox();
			this->s12A1 = new System::Windows::Forms::TextBox();
			this->s10A85 = new System::Windows::Forms::TextBox();
			this->s11E154 = new System::Windows::Forms::TextBox();
			this->s11E153 = new System::Windows::Forms::TextBox();
			this->s11E152 = new System::Windows::Forms::TextBox();
			this->s12A2 = new System::Windows::Forms::TextBox();
			this->s10A96 = new System::Windows::Forms::TextBox();
			this->s11A63 = new System::Windows::Forms::TextBox();
			this->s11A62 = new System::Windows::Forms::TextBox();
			this->s11A61 = new System::Windows::Forms::TextBox();
			this->s11A60 = new System::Windows::Forms::TextBox();
			this->s11A59 = new System::Windows::Forms::TextBox();
			this->s11A54 = new System::Windows::Forms::TextBox();
			this->s11A53 = new System::Windows::Forms::TextBox();
			this->s11A52 = new System::Windows::Forms::TextBox();
			this->s10A91 = new System::Windows::Forms::TextBox();
			this->s10A138 = new System::Windows::Forms::TextBox();
			this->label20 = new System::Windows::Forms::Label();
			this->label19 = new System::Windows::Forms::Label();
			this->label18 = new System::Windows::Forms::Label();
			this->label17 = new System::Windows::Forms::Label();
			this->label14 = new System::Windows::Forms::Label();
			this->label15 = new System::Windows::Forms::Label();
			this->label16 = new System::Windows::Forms::Label();
			this->label11 = new System::Windows::Forms::Label();
			this->label12 = new System::Windows::Forms::Label();
			this->label13 = new System::Windows::Forms::Label();
			this->label10 = new System::Windows::Forms::Label();
			this->label9 = new System::Windows::Forms::Label();
			this->label8 = new System::Windows::Forms::Label();
			this->label7 = new System::Windows::Forms::Label();
			this->label6 = new System::Windows::Forms::Label();
			this->label5 = new System::Windows::Forms::Label();
			this->label4 = new System::Windows::Forms::Label();
			this->label3 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->s10A83 = new System::Windows::Forms::TextBox();
			this->label1 = new System::Windows::Forms::Label();
			this->groupBox2 = new System::Windows::Forms::GroupBox();
			this->s10A149 = new System::Windows::Forms::TextBox();
			this->s11A16 = new System::Windows::Forms::TextBox();
			this->s11A169 = new System::Windows::Forms::TextBox();
			this->label26 = new System::Windows::Forms::Label();
			this->label25 = new System::Windows::Forms::Label();
			this->label24 = new System::Windows::Forms::Label();
			this->s12A12 = new System::Windows::Forms::TextBox();
			this->s12A11 = new System::Windows::Forms::TextBox();
			this->s12A9 = new System::Windows::Forms::TextBox();
			this->label23 = new System::Windows::Forms::Label();
			this->label22 = new System::Windows::Forms::Label();
			this->label21 = new System::Windows::Forms::Label();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->s11E191);
			this->groupBox1->Controls->Add(this->s12A3);
			this->groupBox1->Controls->Add(this->s12A1);
			this->groupBox1->Controls->Add(this->s10A85);
			this->groupBox1->Controls->Add(this->s11E154);
			this->groupBox1->Controls->Add(this->s11E153);
			this->groupBox1->Controls->Add(this->s11E152);
			this->groupBox1->Controls->Add(this->s12A2);
			this->groupBox1->Controls->Add(this->s10A96);
			this->groupBox1->Controls->Add(this->s11A63);
			this->groupBox1->Controls->Add(this->s11A62);
			this->groupBox1->Controls->Add(this->s11A61);
			this->groupBox1->Controls->Add(this->s11A60);
			this->groupBox1->Controls->Add(this->s11A59);
			this->groupBox1->Controls->Add(this->s11A54);
			this->groupBox1->Controls->Add(this->s11A53);
			this->groupBox1->Controls->Add(this->s11A52);
			this->groupBox1->Controls->Add(this->s10A91);
			this->groupBox1->Controls->Add(this->s10A138);
			this->groupBox1->Controls->Add(this->label20);
			this->groupBox1->Controls->Add(this->label19);
			this->groupBox1->Controls->Add(this->label18);
			this->groupBox1->Controls->Add(this->label17);
			this->groupBox1->Controls->Add(this->label14);
			this->groupBox1->Controls->Add(this->label15);
			this->groupBox1->Controls->Add(this->label16);
			this->groupBox1->Controls->Add(this->label11);
			this->groupBox1->Controls->Add(this->label12);
			this->groupBox1->Controls->Add(this->label13);
			this->groupBox1->Controls->Add(this->label10);
			this->groupBox1->Controls->Add(this->label9);
			this->groupBox1->Controls->Add(this->label8);
			this->groupBox1->Controls->Add(this->label7);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->s10A83);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(8, 8);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(248, 504);
			this->groupBox1->TabIndex = 109;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"GUIDANCE && NAVIGATION";
			// 
			// s11E191
			// 
			this->s11E191->Enabled = false;
			this->s11E191->Location = System::Drawing::Point(176, 472);
			this->s11E191->Name = S"s11E191";
			this->s11E191->Size = System::Drawing::Size(64, 20);
			this->s11E191->TabIndex = 148;
			this->s11E191->Text = S"XXXX";
			// 
			// s12A3
			// 
			this->s12A3->Enabled = false;
			this->s12A3->Location = System::Drawing::Point(176, 376);
			this->s12A3->Name = S"s12A3";
			this->s12A3->Size = System::Drawing::Size(64, 20);
			this->s12A3->TabIndex = 147;
			this->s12A3->Text = S"XXXX";
			// 
			// s12A1
			// 
			this->s12A1->Enabled = false;
			this->s12A1->Location = System::Drawing::Point(176, 304);
			this->s12A1->Name = S"s12A1";
			this->s12A1->Size = System::Drawing::Size(64, 20);
			this->s12A1->TabIndex = 146;
			this->s12A1->Text = S"XXXX";
			// 
			// s10A85
			// 
			this->s10A85->Enabled = false;
			this->s10A85->Location = System::Drawing::Point(176, 88);
			this->s10A85->Name = S"s10A85";
			this->s10A85->Size = System::Drawing::Size(64, 20);
			this->s10A85->TabIndex = 145;
			this->s10A85->Text = S"XXX.XX V";
			// 
			// s11E154
			// 
			this->s11E154->Enabled = false;
			this->s11E154->Location = System::Drawing::Point(176, 160);
			this->s11E154->Name = S"s11E154";
			this->s11E154->Size = System::Drawing::Size(64, 20);
			this->s11E154->TabIndex = 144;
			this->s11E154->Text = S"XXXX";
			// 
			// s11E153
			// 
			this->s11E153->Enabled = false;
			this->s11E153->Location = System::Drawing::Point(176, 136);
			this->s11E153->Name = S"s11E153";
			this->s11E153->Size = System::Drawing::Size(64, 20);
			this->s11E153->TabIndex = 143;
			this->s11E153->Text = S"XXXX";
			// 
			// s11E152
			// 
			this->s11E152->Enabled = false;
			this->s11E152->Location = System::Drawing::Point(176, 112);
			this->s11E152->Name = S"s11E152";
			this->s11E152->Size = System::Drawing::Size(64, 20);
			this->s11E152->TabIndex = 142;
			this->s11E152->Text = S"XXXX";
			// 
			// s12A2
			// 
			this->s12A2->Enabled = false;
			this->s12A2->Location = System::Drawing::Point(176, 232);
			this->s12A2->Name = S"s12A2";
			this->s12A2->Size = System::Drawing::Size(64, 20);
			this->s12A2->TabIndex = 141;
			this->s12A2->Text = S"XXXX";
			// 
			// s10A96
			// 
			this->s10A96->Enabled = false;
			this->s10A96->Location = System::Drawing::Point(176, 400);
			this->s10A96->Name = S"s10A96";
			this->s10A96->Size = System::Drawing::Size(64, 20);
			this->s10A96->TabIndex = 140;
			this->s10A96->Text = S"XXX.XX °F";
			// 
			// s11A63
			// 
			this->s11A63->Enabled = false;
			this->s11A63->Location = System::Drawing::Point(176, 352);
			this->s11A63->Name = S"s11A63";
			this->s11A63->Size = System::Drawing::Size(64, 20);
			this->s11A63->TabIndex = 139;
			this->s11A63->Text = S"XXX.XX °";
			// 
			// s11A62
			// 
			this->s11A62->Enabled = false;
			this->s11A62->Location = System::Drawing::Point(176, 328);
			this->s11A62->Name = S"s11A62";
			this->s11A62->Size = System::Drawing::Size(64, 20);
			this->s11A62->TabIndex = 138;
			this->s11A62->Text = S"XXX.XX °";
			// 
			// s11A61
			// 
			this->s11A61->Enabled = false;
			this->s11A61->Location = System::Drawing::Point(176, 280);
			this->s11A61->Name = S"s11A61";
			this->s11A61->Size = System::Drawing::Size(64, 20);
			this->s11A61->TabIndex = 137;
			this->s11A61->Text = S"XXX.XX °";
			// 
			// s11A60
			// 
			this->s11A60->Enabled = false;
			this->s11A60->Location = System::Drawing::Point(176, 256);
			this->s11A60->Name = S"s11A60";
			this->s11A60->Size = System::Drawing::Size(64, 20);
			this->s11A60->TabIndex = 136;
			this->s11A60->Text = S"XXX.XX °";
			// 
			// s11A59
			// 
			this->s11A59->Enabled = false;
			this->s11A59->Location = System::Drawing::Point(176, 208);
			this->s11A59->Name = S"s11A59";
			this->s11A59->Size = System::Drawing::Size(64, 20);
			this->s11A59->TabIndex = 135;
			this->s11A59->Text = S"XXX.XX °";
			// 
			// s11A54
			// 
			this->s11A54->Enabled = false;
			this->s11A54->Location = System::Drawing::Point(176, 184);
			this->s11A54->Name = S"s11A54";
			this->s11A54->Size = System::Drawing::Size(64, 20);
			this->s11A54->TabIndex = 134;
			this->s11A54->Text = S"XXX.XX °";
			// 
			// s11A53
			// 
			this->s11A53->Enabled = false;
			this->s11A53->Location = System::Drawing::Point(176, 448);
			this->s11A53->Name = S"s11A53";
			this->s11A53->Size = System::Drawing::Size(64, 20);
			this->s11A53->TabIndex = 133;
			this->s11A53->Text = S"XXX.XX V";
			// 
			// s11A52
			// 
			this->s11A52->Enabled = false;
			this->s11A52->Location = System::Drawing::Point(176, 424);
			this->s11A52->Name = S"s11A52";
			this->s11A52->Size = System::Drawing::Size(64, 20);
			this->s11A52->TabIndex = 132;
			this->s11A52->Text = S"XXX.XX V";
			// 
			// s10A91
			// 
			this->s10A91->Enabled = false;
			this->s10A91->Location = System::Drawing::Point(176, 64);
			this->s10A91->Name = S"s10A91";
			this->s10A91->Size = System::Drawing::Size(64, 20);
			this->s10A91->TabIndex = 131;
			this->s10A91->Text = S"XXX.XX V";
			// 
			// s10A138
			// 
			this->s10A138->Enabled = false;
			this->s10A138->Location = System::Drawing::Point(176, 40);
			this->s10A138->Name = S"s10A138";
			this->s10A138->Size = System::Drawing::Size(64, 20);
			this->s10A138->TabIndex = 130;
			this->s10A138->Text = S"XXX.XX V";
			// 
			// label20
			// 
			this->label20->Location = System::Drawing::Point(16, 472);
			this->label20->Name = S"label20";
			this->label20->Size = System::Drawing::Size(160, 24);
			this->label20->TabIndex = 129;
			this->label20->Text = S"CMC WARNING";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label19
			// 
			this->label19->Location = System::Drawing::Point(16, 448);
			this->label19->Name = S"label19";
			this->label19->Size = System::Drawing::Size(160, 24);
			this->label19->TabIndex = 128;
			this->label19->Text = S"TRUNNION CDU OUT";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label18
			// 
			this->label18->Location = System::Drawing::Point(16, 424);
			this->label18->Name = S"label18";
			this->label18->Size = System::Drawing::Size(160, 24);
			this->label18->TabIndex = 127;
			this->label18->Text = S"SHAFT CDU DAC OUT";
			this->label18->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label17
			// 
			this->label17->Location = System::Drawing::Point(16, 400);
			this->label17->Name = S"label17";
			this->label17->Size = System::Drawing::Size(160, 24);
			this->label17->TabIndex = 126;
			this->label17->Text = S"PIPA TEMP";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label14
			// 
			this->label14->Location = System::Drawing::Point(16, 376);
			this->label14->Name = S"label14";
			this->label14->Size = System::Drawing::Size(160, 24);
			this->label14->TabIndex = 125;
			this->label14->Text = S"OGA SERVO ERR IN PHASE";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label15
			// 
			this->label15->Location = System::Drawing::Point(16, 352);
			this->label15->Name = S"label15";
			this->label15->Size = System::Drawing::Size(160, 24);
			this->label15->TabIndex = 124;
			this->label15->Text = S"OG 1X RSVR OUT COS";
			this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label16
			// 
			this->label16->Location = System::Drawing::Point(16, 328);
			this->label16->Name = S"label16";
			this->label16->Size = System::Drawing::Size(160, 24);
			this->label16->TabIndex = 123;
			this->label16->Text = S"OG 1X RSVR OUT SIN";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(16, 304);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(160, 24);
			this->label11->TabIndex = 122;
			this->label11->Text = S"MGA SERVO ERR IN PHASE";
			this->label11->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label12
			// 
			this->label12->Location = System::Drawing::Point(16, 280);
			this->label12->Name = S"label12";
			this->label12->Size = System::Drawing::Size(160, 24);
			this->label12->TabIndex = 121;
			this->label12->Text = S"MG 1X RSVR OUT COS";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label13
			// 
			this->label13->Location = System::Drawing::Point(16, 256);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(160, 24);
			this->label13->TabIndex = 120;
			this->label13->Text = S"MG 1X RSVR OUT SIN";
			this->label13->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(16, 232);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(160, 24);
			this->label10->TabIndex = 119;
			this->label10->Text = S"IGA SERVO ERR IN PHASE";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(16, 208);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(160, 24);
			this->label9->TabIndex = 118;
			this->label9->Text = S"IG 1X RSVR OUT COS";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(16, 184);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(160, 24);
			this->label8->TabIndex = 117;
			this->label8->Text = S"IG 1X RSVR OUT SIN";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(16, 160);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(160, 24);
			this->label7->TabIndex = 116;
			this->label7->Text = S"OPTX OPERATE 28 VAC";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(16, 136);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(160, 24);
			this->label6->TabIndex = 115;
			this->label6->Text = S"CMC OPERATE +28 VDC";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(16, 112);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(160, 24);
			this->label5->TabIndex = 114;
			this->label5->Text = S"IMU HTR +28 VDC";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(16, 88);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(160, 24);
			this->label4->TabIndex = 113;
			this->label4->Text = S"3.2KHz 28V SUPPLY";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(16, 64);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(160, 24);
			this->label3->TabIndex = 112;
			this->label3->Text = S"IMU 28 VAC 800Hz";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(16, 40);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(160, 24);
			this->label2->TabIndex = 111;
			this->label2->Text = S"TM BIAS 2.5 VDC";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A83
			// 
			this->s10A83->Enabled = false;
			this->s10A83->Location = System::Drawing::Point(176, 16);
			this->s10A83->Name = S"s10A83";
			this->s10A83->Size = System::Drawing::Size(64, 20);
			this->s10A83->TabIndex = 110;
			this->s10A83->Text = S"XXX.XX V";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(16, 16);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(160, 24);
			this->label1->TabIndex = 109;
			this->label1->Text = S"PIPA +120 VDC";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->s10A149);
			this->groupBox2->Controls->Add(this->s11A16);
			this->groupBox2->Controls->Add(this->s11A169);
			this->groupBox2->Controls->Add(this->label26);
			this->groupBox2->Controls->Add(this->label25);
			this->groupBox2->Controls->Add(this->label24);
			this->groupBox2->Controls->Add(this->s12A12);
			this->groupBox2->Controls->Add(this->s12A11);
			this->groupBox2->Controls->Add(this->s12A9);
			this->groupBox2->Controls->Add(this->label23);
			this->groupBox2->Controls->Add(this->label22);
			this->groupBox2->Controls->Add(this->label21);
			this->groupBox2->Location = System::Drawing::Point(264, 8);
			this->groupBox2->Name = S"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(240, 168);
			this->groupBox2->TabIndex = 110;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = S"FLIGHT TECHNOLOGY";
			// 
			// s10A149
			// 
			this->s10A149->Enabled = false;
			this->s10A149->Location = System::Drawing::Point(168, 136);
			this->s10A149->Name = S"s10A149";
			this->s10A149->Size = System::Drawing::Size(64, 20);
			this->s10A149->TabIndex = 155;
			this->s10A149->Text = S"X.XXX VDC";
			// 
			// s11A16
			// 
			this->s11A16->Enabled = false;
			this->s11A16->Location = System::Drawing::Point(168, 112);
			this->s11A16->Name = S"s11A16";
			this->s11A16->Size = System::Drawing::Size(64, 20);
			this->s11A16->TabIndex = 154;
			this->s11A16->Text = S"XXXX R/H";
			// 
			// s11A169
			// 
			this->s11A169->Enabled = false;
			this->s11A169->Location = System::Drawing::Point(168, 88);
			this->s11A169->Name = S"s11A169";
			this->s11A169->Size = System::Drawing::Size(64, 20);
			this->s11A169->TabIndex = 153;
			this->s11A169->Text = S"XXXX R/H";
			// 
			// label26
			// 
			this->label26->Location = System::Drawing::Point(8, 136);
			this->label26->Name = S"label26";
			this->label26->Size = System::Drawing::Size(160, 24);
			this->label26->TabIndex = 152;
			this->label26->Text = S"DOSIMETER RATE";
			this->label26->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label25
			// 
			this->label25->Location = System::Drawing::Point(8, 112);
			this->label25->Name = S"label25";
			this->label25->Size = System::Drawing::Size(160, 24);
			this->label25->TabIndex = 151;
			this->label25->Text = S"DOSIMETER 2 RADIATION";
			this->label25->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label24
			// 
			this->label24->Location = System::Drawing::Point(8, 88);
			this->label24->Name = S"label24";
			this->label24->Size = System::Drawing::Size(160, 24);
			this->label24->TabIndex = 150;
			this->label24->Text = S"DOSIMETER 1 RADIATION";
			this->label24->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s12A12
			// 
			this->s12A12->Enabled = false;
			this->s12A12->Location = System::Drawing::Point(168, 64);
			this->s12A12->Name = S"s12A12";
			this->s12A12->Size = System::Drawing::Size(64, 20);
			this->s12A12->TabIndex = 149;
			this->s12A12->Text = S"XX.XXX G";
			// 
			// s12A11
			// 
			this->s12A11->Enabled = false;
			this->s12A11->Location = System::Drawing::Point(168, 40);
			this->s12A11->Name = S"s12A11";
			this->s12A11->Size = System::Drawing::Size(64, 20);
			this->s12A11->TabIndex = 148;
			this->s12A11->Text = S"XX.XXX G";
			// 
			// s12A9
			// 
			this->s12A9->Enabled = false;
			this->s12A9->Location = System::Drawing::Point(168, 16);
			this->s12A9->Name = S"s12A9";
			this->s12A9->Size = System::Drawing::Size(64, 20);
			this->s12A9->TabIndex = 147;
			this->s12A9->Text = S"XX.XXX G";
			// 
			// label23
			// 
			this->label23->Location = System::Drawing::Point(8, 64);
			this->label23->Name = S"label23";
			this->label23->Size = System::Drawing::Size(160, 24);
			this->label23->TabIndex = 112;
			this->label23->Text = S"CM Z-AXIS ACCEL";
			this->label23->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label22
			// 
			this->label22->Location = System::Drawing::Point(8, 40);
			this->label22->Name = S"label22";
			this->label22->Size = System::Drawing::Size(160, 24);
			this->label22->TabIndex = 111;
			this->label22->Text = S"CM Y-AXIS ACCEL";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label21
			// 
			this->label21->Location = System::Drawing::Point(8, 16);
			this->label21->Name = S"label21";
			this->label21->Size = System::Drawing::Size(160, 24);
			this->label21->TabIndex = 110;
			this->label21->Text = S"CM X-AXIS ACCEL";
			this->label21->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// GNCForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(510, 516);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = S"GNCForm";
			this->Text = S"GUIDANCE & NAVIGATION / FLIGHT TECHNOLOGY";
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);

		}		

};
}