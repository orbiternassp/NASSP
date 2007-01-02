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
	/// Summary for TelecomForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	__gc class TelecomForm : public System::Windows::Forms::Form
	{
	public: 
		TelecomForm(void)
		{
			InitializeComponent();
			this->add_Closing(new CancelEventHandler(this,TelecomForm_Closing));
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
	private: System::Void TelecomForm_Closing(Object * sender, System::ComponentModel::CancelEventArgs* e){			
			e->Cancel = true; // don't close			
			this->Hide();     // Hide instead.
		}      		
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::Label *  label5;

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
	private: System::Windows::Forms::Label *  label19;
	private: System::Windows::Forms::Label *  label20;
	private: System::Windows::Forms::Label *  label21;
	private: System::Windows::Forms::Label *  label22;
	private: System::Windows::Forms::Label *  label23;
	private: System::Windows::Forms::TextBox *  s11E145;
	private: System::Windows::Forms::TextBox *  s11E328;
	private: System::Windows::Forms::TextBox *  s11A148;
	private: System::Windows::Forms::TextBox *  s11A154;
	private: System::Windows::Forms::TextBox *  s11A119;
	private: System::Windows::Forms::TextBox *  s11A120;
	private: System::Windows::Forms::TextBox *  s11A48;
	private: System::Windows::Forms::TextBox *  s11A49;
	private: System::Windows::Forms::TextBox *  s11D2;
	private: System::Windows::Forms::TextBox *  s51D;
	private: System::Windows::Forms::TextBox *  s11A121;
	private: System::Windows::Forms::TextBox *  s11A50;
	private: System::Windows::Forms::TextBox *  s11A17;
	private: System::Windows::Forms::TextBox *  s11A18;
	private: System::Windows::Forms::TextBox *  s11A19;
	private: System::Windows::Forms::TextBox *  s11A41;
	private: System::Windows::Forms::TextBox *  s11A38;
	private: System::Windows::Forms::TextBox *  s11A20;
	private: System::Windows::Forms::TextBox *  s11A43;
	private: System::Windows::Forms::TextBox *  s11A42;
	private: System::Windows::Forms::TextBox *  s10A61;
	private: System::Windows::Forms::TextBox *  s10A62;






















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
			this->label1 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->label3 = new System::Windows::Forms::Label();
			this->label4 = new System::Windows::Forms::Label();
			this->label5 = new System::Windows::Forms::Label();
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
			this->label19 = new System::Windows::Forms::Label();
			this->label20 = new System::Windows::Forms::Label();
			this->label21 = new System::Windows::Forms::Label();
			this->label22 = new System::Windows::Forms::Label();
			this->label23 = new System::Windows::Forms::Label();
			this->s11E145 = new System::Windows::Forms::TextBox();
			this->s11E328 = new System::Windows::Forms::TextBox();
			this->s11A148 = new System::Windows::Forms::TextBox();
			this->s11A154 = new System::Windows::Forms::TextBox();
			this->s11A119 = new System::Windows::Forms::TextBox();
			this->s11A120 = new System::Windows::Forms::TextBox();
			this->s11A48 = new System::Windows::Forms::TextBox();
			this->s11A49 = new System::Windows::Forms::TextBox();
			this->s11D2 = new System::Windows::Forms::TextBox();
			this->s51D = new System::Windows::Forms::TextBox();
			this->s11A121 = new System::Windows::Forms::TextBox();
			this->s11A50 = new System::Windows::Forms::TextBox();
			this->s11A17 = new System::Windows::Forms::TextBox();
			this->s11A18 = new System::Windows::Forms::TextBox();
			this->s11A19 = new System::Windows::Forms::TextBox();
			this->s11A41 = new System::Windows::Forms::TextBox();
			this->s11A38 = new System::Windows::Forms::TextBox();
			this->s11A20 = new System::Windows::Forms::TextBox();
			this->s11A43 = new System::Windows::Forms::TextBox();
			this->s11A42 = new System::Windows::Forms::TextBox();
			this->s10A61 = new System::Windows::Forms::TextBox();
			this->s10A62 = new System::Windows::Forms::TextBox();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 8);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(184, 24);
			this->label1->TabIndex = 4;
			this->label1->Text = S"DSE TAPE MOTION";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 32);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(184, 24);
			this->label2->TabIndex = 5;
			this->label2->Text = S"SCE POS SUPPLY VOLTS";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 56);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(184, 24);
			this->label3->TabIndex = 6;
			this->label3->Text = S"SCE NEG SUPPLY VOLTS";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(8, 80);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(184, 24);
			this->label4->TabIndex = 7;
			this->label4->Text = S"SENSOR EXCITATION 5 VOLTS";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(8, 104);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(184, 24);
			this->label5->TabIndex = 8;
			this->label5->Text = S"SENSOR EXCITATION 10 VOLTS";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(8, 128);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(184, 24);
			this->label7->TabIndex = 10;
			this->label7->Text = S"PCM HIGH LEVEL 85 PCT REF";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(8, 152);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(184, 24);
			this->label8->TabIndex = 11;
			this->label8->Text = S"PCM HIGH LEVEL 15 PCT REF";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(96, 272);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(216, 24);
			this->label9->TabIndex = 12;
			this->label9->Text = S"CTE TIME FROM LAUNCH 32-BIT";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 176);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(184, 24);
			this->label10->TabIndex = 13;
			this->label10->Text = S"UDL VALIDITY SIGNAL 4-BIT";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(8, 200);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(184, 24);
			this->label11->TabIndex = 14;
			this->label11->Text = S"PCM SYNC SOURCE EXT OR INT";
			this->label11->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label12
			// 
			this->label12->Location = System::Drawing::Point(8, 224);
			this->label12->Name = S"label12";
			this->label12->Size = System::Drawing::Size(184, 24);
			this->label12->TabIndex = 15;
			this->label12->Text = S"USB RCVR AGC VOLTAGE";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label13
			// 
			this->label13->Location = System::Drawing::Point(8, 248);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(184, 24);
			this->label13->TabIndex = 16;
			this->label13->Text = S"USB RCVR PHASE ERROR";
			this->label13->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label14
			// 
			this->label14->Location = System::Drawing::Point(272, 8);
			this->label14->Name = S"label14";
			this->label14->Size = System::Drawing::Size(184, 24);
			this->label14->TabIndex = 17;
			this->label14->Text = S"PROTON CT RATE CHAN 1";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label15
			// 
			this->label15->Location = System::Drawing::Point(272, 32);
			this->label15->Name = S"label15";
			this->label15->Size = System::Drawing::Size(184, 24);
			this->label15->TabIndex = 18;
			this->label15->Text = S"PROTON CT RATE CHAN 2";
			this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label16
			// 
			this->label16->Location = System::Drawing::Point(272, 56);
			this->label16->Name = S"label16";
			this->label16->Size = System::Drawing::Size(184, 24);
			this->label16->TabIndex = 19;
			this->label16->Text = S"PROTON CT RATE CHAN 3";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label17
			// 
			this->label17->Location = System::Drawing::Point(272, 80);
			this->label17->Name = S"label17";
			this->label17->Size = System::Drawing::Size(184, 24);
			this->label17->TabIndex = 20;
			this->label17->Text = S"PROTON CT RATE CHAN 4";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label18
			// 
			this->label18->Location = System::Drawing::Point(272, 104);
			this->label18->Name = S"label18";
			this->label18->Size = System::Drawing::Size(184, 24);
			this->label18->TabIndex = 21;
			this->label18->Text = S"ALPHA CT RATE CHAN 1";
			this->label18->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label19
			// 
			this->label19->Location = System::Drawing::Point(272, 128);
			this->label19->Name = S"label19";
			this->label19->Size = System::Drawing::Size(184, 24);
			this->label19->TabIndex = 22;
			this->label19->Text = S"ALPHA CT RATE CHAN 2";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label20
			// 
			this->label20->Location = System::Drawing::Point(272, 152);
			this->label20->Name = S"label20";
			this->label20->Size = System::Drawing::Size(184, 24);
			this->label20->TabIndex = 23;
			this->label20->Text = S"ALPHA CT RATE CHAN 3";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label21
			// 
			this->label21->Location = System::Drawing::Point(272, 176);
			this->label21->Name = S"label21";
			this->label21->Size = System::Drawing::Size(184, 24);
			this->label21->TabIndex = 24;
			this->label21->Text = S"PROTON INTEG CT RATE";
			this->label21->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label22
			// 
			this->label22->Location = System::Drawing::Point(272, 200);
			this->label22->Name = S"label22";
			this->label22->Size = System::Drawing::Size(184, 24);
			this->label22->TabIndex = 25;
			this->label22->Text = S"NUCLEAR PARTICLE DET TEMP";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label23
			// 
			this->label23->Location = System::Drawing::Point(272, 224);
			this->label23->Name = S"label23";
			this->label23->Size = System::Drawing::Size(184, 24);
			this->label23->TabIndex = 26;
			this->label23->Text = S"NUCLEAR PARTICLE ANAL TEMP";
			this->label23->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s11E145
			// 
			this->s11E145->Enabled = false;
			this->s11E145->Location = System::Drawing::Point(192, 8);
			this->s11E145->Name = S"s11E145";
			this->s11E145->Size = System::Drawing::Size(72, 20);
			this->s11E145->TabIndex = 101;
			this->s11E145->Text = S"XXXX";
			// 
			// s11E328
			// 
			this->s11E328->Enabled = false;
			this->s11E328->Location = System::Drawing::Point(192, 200);
			this->s11E328->Name = S"s11E328";
			this->s11E328->Size = System::Drawing::Size(72, 20);
			this->s11E328->TabIndex = 102;
			this->s11E328->Text = S"XXXX";
			// 
			// s11A148
			// 
			this->s11A148->Enabled = false;
			this->s11A148->Location = System::Drawing::Point(192, 32);
			this->s11A148->Name = S"s11A148";
			this->s11A148->Size = System::Drawing::Size(72, 20);
			this->s11A148->TabIndex = 103;
			this->s11A148->Text = S"XX.XX V";
			// 
			// s11A154
			// 
			this->s11A154->Enabled = false;
			this->s11A154->Location = System::Drawing::Point(192, 56);
			this->s11A154->Name = S"s11A154";
			this->s11A154->Size = System::Drawing::Size(72, 20);
			this->s11A154->TabIndex = 104;
			this->s11A154->Text = S"XX.XX V";
			// 
			// s11A119
			// 
			this->s11A119->Enabled = false;
			this->s11A119->Location = System::Drawing::Point(192, 80);
			this->s11A119->Name = S"s11A119";
			this->s11A119->Size = System::Drawing::Size(72, 20);
			this->s11A119->TabIndex = 105;
			this->s11A119->Text = S"XX.XX V";
			// 
			// s11A120
			// 
			this->s11A120->Enabled = false;
			this->s11A120->Location = System::Drawing::Point(192, 104);
			this->s11A120->Name = S"s11A120";
			this->s11A120->Size = System::Drawing::Size(72, 20);
			this->s11A120->TabIndex = 106;
			this->s11A120->Text = S"XX.XX V";
			// 
			// s11A48
			// 
			this->s11A48->Enabled = false;
			this->s11A48->Location = System::Drawing::Point(192, 128);
			this->s11A48->Name = S"s11A48";
			this->s11A48->Size = System::Drawing::Size(72, 20);
			this->s11A48->TabIndex = 107;
			this->s11A48->Text = S"XX.XX V";
			// 
			// s11A49
			// 
			this->s11A49->Enabled = false;
			this->s11A49->Location = System::Drawing::Point(192, 152);
			this->s11A49->Name = S"s11A49";
			this->s11A49->Size = System::Drawing::Size(72, 20);
			this->s11A49->TabIndex = 108;
			this->s11A49->Text = S"XX.XX V";
			// 
			// s11D2
			// 
			this->s11D2->Enabled = false;
			this->s11D2->Location = System::Drawing::Point(280, 272);
			this->s11D2->Name = S"s11D2";
			this->s11D2->Size = System::Drawing::Size(112, 20);
			this->s11D2->TabIndex = 109;
			this->s11D2->Text = S"DD:HH:MM:SS.XXXX";
			// 
			// s51D
			// 
			this->s51D->Enabled = false;
			this->s51D->Location = System::Drawing::Point(192, 176);
			this->s51D->Name = S"s51D";
			this->s51D->Size = System::Drawing::Size(72, 20);
			this->s51D->TabIndex = 110;
			this->s51D->Text = S"XXXX";
			// 
			// s11A121
			// 
			this->s11A121->Enabled = false;
			this->s11A121->Location = System::Drawing::Point(192, 224);
			this->s11A121->Name = S"s11A121";
			this->s11A121->Size = System::Drawing::Size(72, 20);
			this->s11A121->TabIndex = 111;
			this->s11A121->Text = S"-XXX DBM";
			// 
			// s11A50
			// 
			this->s11A50->Enabled = false;
			this->s11A50->Location = System::Drawing::Point(192, 248);
			this->s11A50->Name = S"s11A50";
			this->s11A50->Size = System::Drawing::Size(72, 20);
			this->s11A50->TabIndex = 112;
			this->s11A50->Text = S"XXXXXX Hz";
			// 
			// s11A17
			// 
			this->s11A17->Enabled = false;
			this->s11A17->Location = System::Drawing::Point(456, 8);
			this->s11A17->Name = S"s11A17";
			this->s11A17->Size = System::Drawing::Size(72, 20);
			this->s11A17->TabIndex = 113;
			this->s11A17->Text = S"XXXXXX C/S";
			// 
			// s11A18
			// 
			this->s11A18->Enabled = false;
			this->s11A18->Location = System::Drawing::Point(456, 32);
			this->s11A18->Name = S"s11A18";
			this->s11A18->Size = System::Drawing::Size(72, 20);
			this->s11A18->TabIndex = 114;
			this->s11A18->Text = S"XXXXXX C/S";
			// 
			// s11A19
			// 
			this->s11A19->Enabled = false;
			this->s11A19->Location = System::Drawing::Point(456, 56);
			this->s11A19->Name = S"s11A19";
			this->s11A19->Size = System::Drawing::Size(72, 20);
			this->s11A19->TabIndex = 115;
			this->s11A19->Text = S"XXXXXX C/S";
			// 
			// s11A41
			// 
			this->s11A41->Enabled = false;
			this->s11A41->Location = System::Drawing::Point(456, 128);
			this->s11A41->Name = S"s11A41";
			this->s11A41->Size = System::Drawing::Size(72, 20);
			this->s11A41->TabIndex = 118;
			this->s11A41->Text = S"XXXXXX C/S";
			// 
			// s11A38
			// 
			this->s11A38->Enabled = false;
			this->s11A38->Location = System::Drawing::Point(456, 104);
			this->s11A38->Name = S"s11A38";
			this->s11A38->Size = System::Drawing::Size(72, 20);
			this->s11A38->TabIndex = 117;
			this->s11A38->Text = S"XXXXXX C/S";
			// 
			// s11A20
			// 
			this->s11A20->Enabled = false;
			this->s11A20->Location = System::Drawing::Point(456, 80);
			this->s11A20->Name = S"s11A20";
			this->s11A20->Size = System::Drawing::Size(72, 20);
			this->s11A20->TabIndex = 116;
			this->s11A20->Text = S"XXXXXX C/S";
			// 
			// s11A43
			// 
			this->s11A43->Enabled = false;
			this->s11A43->Location = System::Drawing::Point(456, 176);
			this->s11A43->Name = S"s11A43";
			this->s11A43->Size = System::Drawing::Size(72, 20);
			this->s11A43->TabIndex = 120;
			this->s11A43->Text = S"XXXXXX C/S";
			// 
			// s11A42
			// 
			this->s11A42->Enabled = false;
			this->s11A42->Location = System::Drawing::Point(456, 152);
			this->s11A42->Name = S"s11A42";
			this->s11A42->Size = System::Drawing::Size(72, 20);
			this->s11A42->TabIndex = 119;
			this->s11A42->Text = S"XXXXXX C/S";
			// 
			// s10A61
			// 
			this->s10A61->Enabled = false;
			this->s10A61->Location = System::Drawing::Point(456, 200);
			this->s10A61->Name = S"s10A61";
			this->s10A61->Size = System::Drawing::Size(72, 20);
			this->s10A61->TabIndex = 121;
			this->s10A61->Text = S"XXX.XX °F";
			// 
			// s10A62
			// 
			this->s10A62->Enabled = false;
			this->s10A62->Location = System::Drawing::Point(456, 224);
			this->s10A62->Name = S"s10A62";
			this->s10A62->Size = System::Drawing::Size(72, 20);
			this->s10A62->TabIndex = 122;
			this->s10A62->Text = S"XXX.XX °F";
			// 
			// TelecomForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(534, 300);
			this->Controls->Add(this->s10A62);
			this->Controls->Add(this->s10A61);
			this->Controls->Add(this->s11A43);
			this->Controls->Add(this->s11A42);
			this->Controls->Add(this->s11A41);
			this->Controls->Add(this->s11A38);
			this->Controls->Add(this->s11A20);
			this->Controls->Add(this->s11A19);
			this->Controls->Add(this->s11A18);
			this->Controls->Add(this->s11A17);
			this->Controls->Add(this->s11A50);
			this->Controls->Add(this->s11A121);
			this->Controls->Add(this->s51D);
			this->Controls->Add(this->s11D2);
			this->Controls->Add(this->s11A49);
			this->Controls->Add(this->s11A48);
			this->Controls->Add(this->s11A120);
			this->Controls->Add(this->s11A119);
			this->Controls->Add(this->s11A154);
			this->Controls->Add(this->s11A148);
			this->Controls->Add(this->s11E328);
			this->Controls->Add(this->s11E145);
			this->Controls->Add(this->label23);
			this->Controls->Add(this->label22);
			this->Controls->Add(this->label21);
			this->Controls->Add(this->label20);
			this->Controls->Add(this->label19);
			this->Controls->Add(this->label18);
			this->Controls->Add(this->label17);
			this->Controls->Add(this->label16);
			this->Controls->Add(this->label15);
			this->Controls->Add(this->label14);
			this->Controls->Add(this->label13);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = S"TelecomForm";
			this->Text = S"COMMUNICATION AND INSTRUMENTATION";
			this->ResumeLayout(false);

		}		

};
}