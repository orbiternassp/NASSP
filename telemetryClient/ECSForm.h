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
	/// Summary for ECSForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	__gc class ECSForm : public System::Windows::Forms::Form
	{
	public: 
		ECSForm(void)
		{
			InitializeComponent();
			this->add_Closing(new CancelEventHandler(this, &GroundStation::ECSForm::ECSForm_Closing));
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
	private: System::Void ECSForm_Closing(Object * sender, System::ComponentModel::CancelEventArgs* e){			
			e->Cancel = true; // don't close			
			this->Hide();     // Hide instead.
		}      		
	private: System::Windows::Forms::Label *  label19;
	private: System::Windows::Forms::TextBox *  s10A6;
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
	private: System::Windows::Forms::Label *  label22;
	private: System::Windows::Forms::Label *  label23;
	private: System::Windows::Forms::Label *  label24;
	private: System::Windows::Forms::Label *  label25;
	private: System::Windows::Forms::Label *  label26;
	private: System::Windows::Forms::Label *  label27;
	private: System::Windows::Forms::Label *  label28;
	public: System::Windows::Forms::TextBox *  s11A4;
	public: System::Windows::Forms::TextBox *  s11A1;
	public: System::Windows::Forms::TextBox *  s10A4;
	public: System::Windows::Forms::TextBox *  s10A101;
	public: System::Windows::Forms::TextBox *  s10A84;
	public: System::Windows::Forms::TextBox *  s10A133;
	public: System::Windows::Forms::TextBox *  s10A12;
	public: System::Windows::Forms::TextBox *  s10A15;
	public: System::Windows::Forms::TextBox *  s11A118;
	public: System::Windows::Forms::TextBox *  s10A100;
	public: System::Windows::Forms::TextBox *  s10A48;
	public: System::Windows::Forms::TextBox *  s10A87;
	public: System::Windows::Forms::TextBox *  s10A90;
	public: System::Windows::Forms::TextBox *  s11A37;
	public: System::Windows::Forms::TextBox *  s10A3;
	public: System::Windows::Forms::TextBox *  s10A81;
	public: System::Windows::Forms::TextBox *  s10A9;
	public: System::Windows::Forms::TextBox *  s10A45;
	public: System::Windows::Forms::TextBox *  s11A2;
	public: System::Windows::Forms::TextBox *  s11A3;
	public: System::Windows::Forms::TextBox *  s11A13;
	public: System::Windows::Forms::TextBox *  s11A14;
	public: System::Windows::Forms::TextBox *  s11A55;
	public: System::Windows::Forms::TextBox *  s11A157;
	public: System::Windows::Forms::TextBox *  s11A129;
	public: System::Windows::Forms::TextBox *  s10A8;
	public: System::Windows::Forms::TextBox *  s11A87;
	private: System::Windows::Forms::Label *  label29;
	private: System::Windows::Forms::Label *  label30;
	private: System::Windows::Forms::Label *  label31;
	private: System::Windows::Forms::TextBox *  s10A135;
	private: System::Windows::Forms::TextBox *  s10A114;
	private: System::Windows::Forms::TextBox *  s11E4_8;






























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
			this->s10A6 = new System::Windows::Forms::TextBox();
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
			this->label22 = new System::Windows::Forms::Label();
			this->label23 = new System::Windows::Forms::Label();
			this->label24 = new System::Windows::Forms::Label();
			this->label25 = new System::Windows::Forms::Label();
			this->label26 = new System::Windows::Forms::Label();
			this->label27 = new System::Windows::Forms::Label();
			this->label28 = new System::Windows::Forms::Label();
			this->s11A4 = new System::Windows::Forms::TextBox();
			this->s11A1 = new System::Windows::Forms::TextBox();
			this->s10A4 = new System::Windows::Forms::TextBox();
			this->s10A101 = new System::Windows::Forms::TextBox();
			this->s10A84 = new System::Windows::Forms::TextBox();
			this->s10A133 = new System::Windows::Forms::TextBox();
			this->s10A12 = new System::Windows::Forms::TextBox();
			this->s10A15 = new System::Windows::Forms::TextBox();
			this->s11A118 = new System::Windows::Forms::TextBox();
			this->s10A100 = new System::Windows::Forms::TextBox();
			this->s10A48 = new System::Windows::Forms::TextBox();
			this->s10A87 = new System::Windows::Forms::TextBox();
			this->s10A90 = new System::Windows::Forms::TextBox();
			this->s11A37 = new System::Windows::Forms::TextBox();
			this->s10A3 = new System::Windows::Forms::TextBox();
			this->s10A81 = new System::Windows::Forms::TextBox();
			this->s10A9 = new System::Windows::Forms::TextBox();
			this->s10A45 = new System::Windows::Forms::TextBox();
			this->s11A2 = new System::Windows::Forms::TextBox();
			this->s11A3 = new System::Windows::Forms::TextBox();
			this->s11A13 = new System::Windows::Forms::TextBox();
			this->s11A14 = new System::Windows::Forms::TextBox();
			this->s11A55 = new System::Windows::Forms::TextBox();
			this->s11A157 = new System::Windows::Forms::TextBox();
			this->s11A129 = new System::Windows::Forms::TextBox();
			this->s10A8 = new System::Windows::Forms::TextBox();
			this->s11A87 = new System::Windows::Forms::TextBox();
			this->label29 = new System::Windows::Forms::Label();
			this->label30 = new System::Windows::Forms::Label();
			this->label31 = new System::Windows::Forms::Label();
			this->s10A135 = new System::Windows::Forms::TextBox();
			this->s10A114 = new System::Windows::Forms::TextBox();
			this->s11E4_8 = new System::Windows::Forms::TextBox();
			this->SuspendLayout();
			// 
			// label19
			// 
			this->label19->Location = System::Drawing::Point(8, 8);
			this->label19->Name = S"label19";
			this->label19->Size = System::Drawing::Size(176, 24);
			this->label19->TabIndex = 40;
			this->label19->Text = S"CABIN PRESS";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A6
			// 
			this->s10A6->Enabled = false;
			this->s10A6->Location = System::Drawing::Point(192, 8);
			this->s10A6->Name = S"s10A6";
			this->s10A6->Size = System::Drawing::Size(64, 20);
			this->s10A6->TabIndex = 74;
			this->s10A6->Text = S"XXXX PSIA";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 32);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(176, 24);
			this->label1->TabIndex = 75;
			this->label1->Text = S"CABIN TEMP";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 56);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(176, 24);
			this->label2->TabIndex = 76;
			this->label2->Text = S"SUIT-CABIN DELTA PRESS";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 80);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(176, 24);
			this->label3->TabIndex = 77;
			this->label3->Text = S"CO2 PARTIAL PRESS";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(8, 104);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(176, 24);
			this->label4->TabIndex = 78;
			this->label4->Text = S"SURGE TANK PRESS";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(8, 128);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(176, 24);
			this->label5->TabIndex = 79;
			this->label5->Text = S"SUIT AIR HX OUT TEMP";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(8, 152);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(176, 24);
			this->label6->TabIndex = 80;
			this->label6->Text = S"WASTE H2O QTY";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(8, 176);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(176, 24);
			this->label7->TabIndex = 81;
			this->label7->Text = S"POTABLE H2O QTY";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(8, 200);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(176, 24);
			this->label8->TabIndex = 82;
			this->label8->Text = S"SUIT MANF ABS PRESS";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(8, 224);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(176, 24);
			this->label9->TabIndex = 83;
			this->label9->Text = S"SUIT COMP DELTA P";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 248);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(176, 24);
			this->label10->TabIndex = 84;
			this->label10->Text = S"GLY PUMP OUT PRESS";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(8, 272);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(176, 24);
			this->label11->TabIndex = 85;
			this->label11->Text = S"GLY EVAP OUT STEAM TEMP";
			this->label11->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label12
			// 
			this->label12->Location = System::Drawing::Point(8, 296);
			this->label12->Name = S"label12";
			this->label12->Size = System::Drawing::Size(176, 24);
			this->label12->TabIndex = 86;
			this->label12->Text = S"GLY EVAP OUT TEMP";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label13
			// 
			this->label13->Location = System::Drawing::Point(8, 320);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(176, 24);
			this->label13->TabIndex = 87;
			this->label13->Text = S"GLY ACCUM QTY";
			this->label13->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label14
			// 
			this->label14->Location = System::Drawing::Point(8, 344);
			this->label14->Name = S"label14";
			this->label14->Size = System::Drawing::Size(176, 24);
			this->label14->TabIndex = 88;
			this->label14->Text = S"ECS RAD OUT TEMP";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label15
			// 
			this->label15->Location = System::Drawing::Point(264, 8);
			this->label15->Name = S"label15";
			this->label15->Size = System::Drawing::Size(176, 24);
			this->label15->TabIndex = 89;
			this->label15->Text = S"GLY EVAP BACK PRESS";
			this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label16
			// 
			this->label16->Location = System::Drawing::Point(264, 32);
			this->label16->Name = S"label16";
			this->label16->Size = System::Drawing::Size(184, 24);
			this->label16->TabIndex = 90;
			this->label16->Text = S"ECS O2 FLOW O2 SUPPLY MANF";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label17
			// 
			this->label17->Location = System::Drawing::Point(264, 56);
			this->label17->Name = S"label17";
			this->label17->Size = System::Drawing::Size(176, 24);
			this->label17->TabIndex = 91;
			this->label17->Text = S"O2 SUPPLY MANF PRESS";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label18
			// 
			this->label18->Location = System::Drawing::Point(264, 80);
			this->label18->Name = S"label18";
			this->label18->Size = System::Drawing::Size(176, 24);
			this->label18->TabIndex = 92;
			this->label18->Text = S"SEC GLY PUMP OUT PRESS";
			this->label18->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label20
			// 
			this->label20->Location = System::Drawing::Point(264, 104);
			this->label20->Name = S"label20";
			this->label20->Size = System::Drawing::Size(176, 24);
			this->label20->TabIndex = 93;
			this->label20->Text = S"SEC EVAP OUT LIQ TEMP";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label21
			// 
			this->label21->Location = System::Drawing::Point(264, 128);
			this->label21->Name = S"label21";
			this->label21->Size = System::Drawing::Size(176, 24);
			this->label21->TabIndex = 94;
			this->label21->Text = S"SEC GLY ACCUM QTY";
			this->label21->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label22
			// 
			this->label22->Location = System::Drawing::Point(264, 152);
			this->label22->Name = S"label22";
			this->label22->Size = System::Drawing::Size(176, 24);
			this->label22->TabIndex = 95;
			this->label22->Text = S"SEC EVAP OUT STEAM PRESS";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label23
			// 
			this->label23->Location = System::Drawing::Point(264, 176);
			this->label23->Name = S"label23";
			this->label23->Size = System::Drawing::Size(192, 24);
			this->label23->TabIndex = 96;
			this->label23->Text = S"H2O TANK - GLY RES PRESS";
			this->label23->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label24
			// 
			this->label24->Location = System::Drawing::Point(264, 200);
			this->label24->Name = S"label24";
			this->label24->Size = System::Drawing::Size(192, 24);
			this->label24->TabIndex = 97;
			this->label24->Text = S"PRI GLY FLOW RATE";
			this->label24->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label25
			// 
			this->label25->Location = System::Drawing::Point(264, 224);
			this->label25->Name = S"label25";
			this->label25->Size = System::Drawing::Size(192, 24);
			this->label25->TabIndex = 98;
			this->label25->Text = S"PRI EVAP INLET TEMP";
			this->label25->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label26
			// 
			this->label26->Location = System::Drawing::Point(264, 248);
			this->label26->Name = S"label26";
			this->label26->Size = System::Drawing::Size(192, 24);
			this->label26->TabIndex = 99;
			this->label26->Text = S"PRI RAD IN TEMP";
			this->label26->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label27
			// 
			this->label27->Location = System::Drawing::Point(264, 272);
			this->label27->Name = S"label27";
			this->label27->Size = System::Drawing::Size(192, 24);
			this->label27->TabIndex = 100;
			this->label27->Text = S"SEC RAD IN TEMP";
			this->label27->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label28
			// 
			this->label28->Location = System::Drawing::Point(264, 296);
			this->label28->Name = S"label28";
			this->label28->Size = System::Drawing::Size(192, 24);
			this->label28->TabIndex = 101;
			this->label28->Text = S"SEC RAD OUT TEMP";
			this->label28->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s11A4
			// 
			this->s11A4->Enabled = false;
			this->s11A4->Location = System::Drawing::Point(192, 104);
			this->s11A4->Name = S"s11A4";
			this->s11A4->Size = System::Drawing::Size(64, 20);
			this->s11A4->TabIndex = 102;
			this->s11A4->Text = S"XXXX PSIA";
			// 
			// s11A1
			// 
			this->s11A1->Enabled = false;
			this->s11A1->Location = System::Drawing::Point(192, 200);
			this->s11A1->Name = S"s11A1";
			this->s11A1->Size = System::Drawing::Size(64, 20);
			this->s11A1->TabIndex = 103;
			this->s11A1->Text = S"XXXX PSIA";
			// 
			// s10A4
			// 
			this->s10A4->Enabled = false;
			this->s10A4->Location = System::Drawing::Point(456, 8);
			this->s10A4->Name = S"s10A4";
			this->s10A4->Size = System::Drawing::Size(64, 20);
			this->s10A4->TabIndex = 104;
			this->s10A4->Text = S"XXXX PSIA";
			// 
			// s10A101
			// 
			this->s10A101->Enabled = false;
			this->s10A101->Location = System::Drawing::Point(456, 176);
			this->s10A101->Name = S"s10A101";
			this->s10A101->Size = System::Drawing::Size(64, 20);
			this->s10A101->TabIndex = 105;
			this->s10A101->Text = S"XXXX PSIA";
			// 
			// s10A84
			// 
			this->s10A84->Enabled = false;
			this->s10A84->Location = System::Drawing::Point(192, 32);
			this->s10A84->Name = S"s10A84";
			this->s10A84->Size = System::Drawing::Size(64, 20);
			this->s10A84->TabIndex = 106;
			this->s10A84->Text = S"XXX.XX °F";
			// 
			// s10A133
			// 
			this->s10A133->Enabled = false;
			this->s10A133->Location = System::Drawing::Point(192, 272);
			this->s10A133->Name = S"s10A133";
			this->s10A133->Size = System::Drawing::Size(64, 20);
			this->s10A133->TabIndex = 107;
			this->s10A133->Text = S"XXX.XX °F";
			// 
			// s10A12
			// 
			this->s10A12->Enabled = false;
			this->s10A12->Location = System::Drawing::Point(192, 296);
			this->s10A12->Name = S"s10A12";
			this->s10A12->Size = System::Drawing::Size(64, 20);
			this->s10A12->TabIndex = 108;
			this->s10A12->Text = S"XXX.XX °F";
			// 
			// s10A15
			// 
			this->s10A15->Enabled = false;
			this->s10A15->Location = System::Drawing::Point(192, 344);
			this->s10A15->Name = S"s10A15";
			this->s10A15->Size = System::Drawing::Size(64, 20);
			this->s10A15->TabIndex = 109;
			this->s10A15->Text = S"XXX.XX °F";
			// 
			// s11A118
			// 
			this->s11A118->Enabled = false;
			this->s11A118->Location = System::Drawing::Point(456, 104);
			this->s11A118->Name = S"s11A118";
			this->s11A118->Size = System::Drawing::Size(64, 20);
			this->s11A118->TabIndex = 110;
			this->s11A118->Text = S"XXX.XX °F";
			// 
			// s10A100
			// 
			this->s10A100->Enabled = false;
			this->s10A100->Location = System::Drawing::Point(456, 224);
			this->s10A100->Name = S"s10A100";
			this->s10A100->Size = System::Drawing::Size(64, 20);
			this->s10A100->TabIndex = 111;
			this->s10A100->Text = S"XXX.XX °F";
			// 
			// s10A48
			// 
			this->s10A48->Enabled = false;
			this->s10A48->Location = System::Drawing::Point(456, 248);
			this->s10A48->Name = S"s10A48";
			this->s10A48->Size = System::Drawing::Size(64, 20);
			this->s10A48->TabIndex = 112;
			this->s10A48->Text = S"XXX.XX °F";
			// 
			// s10A87
			// 
			this->s10A87->Enabled = false;
			this->s10A87->Location = System::Drawing::Point(456, 272);
			this->s10A87->Name = S"s10A87";
			this->s10A87->Size = System::Drawing::Size(64, 20);
			this->s10A87->TabIndex = 113;
			this->s10A87->Text = S"XXX.XX °F";
			// 
			// s10A90
			// 
			this->s10A90->Enabled = false;
			this->s10A90->Location = System::Drawing::Point(456, 296);
			this->s10A90->Name = S"s10A90";
			this->s10A90->Size = System::Drawing::Size(64, 20);
			this->s10A90->TabIndex = 114;
			this->s10A90->Text = S"XXX.XX °F";
			// 
			// s11A37
			// 
			this->s11A37->Enabled = false;
			this->s11A37->Location = System::Drawing::Point(192, 56);
			this->s11A37->Name = S"s11A37";
			this->s11A37->Size = System::Drawing::Size(64, 20);
			this->s11A37->TabIndex = 115;
			this->s11A37->Text = S"XXXX IN";
			// 
			// s10A3
			// 
			this->s10A3->Enabled = false;
			this->s10A3->Location = System::Drawing::Point(192, 80);
			this->s10A3->Name = S"s10A3";
			this->s10A3->Size = System::Drawing::Size(64, 20);
			this->s10A3->TabIndex = 116;
			this->s10A3->Text = S"XXXX MM";
			// 
			// s10A81
			// 
			this->s10A81->Enabled = false;
			this->s10A81->Location = System::Drawing::Point(192, 176);
			this->s10A81->Name = S"s10A81";
			this->s10A81->Size = System::Drawing::Size(64, 20);
			this->s10A81->TabIndex = 117;
			this->s10A81->Text = S"XXX.XX%";
			// 
			// s10A9
			// 
			this->s10A9->Enabled = false;
			this->s10A9->Location = System::Drawing::Point(192, 152);
			this->s10A9->Name = S"s10A9";
			this->s10A9->Size = System::Drawing::Size(64, 20);
			this->s10A9->TabIndex = 118;
			this->s10A9->Text = S"XXX.XX%";
			// 
			// s10A45
			// 
			this->s10A45->Enabled = false;
			this->s10A45->Location = System::Drawing::Point(192, 128);
			this->s10A45->Name = S"s10A45";
			this->s10A45->Size = System::Drawing::Size(64, 20);
			this->s10A45->TabIndex = 119;
			this->s10A45->Text = S"XXX.XX °F";
			// 
			// s11A2
			// 
			this->s11A2->Enabled = false;
			this->s11A2->Location = System::Drawing::Point(192, 224);
			this->s11A2->Name = S"s11A2";
			this->s11A2->Size = System::Drawing::Size(64, 20);
			this->s11A2->TabIndex = 120;
			this->s11A2->Text = S"XXXX PSID";
			// 
			// s11A3
			// 
			this->s11A3->Enabled = false;
			this->s11A3->Location = System::Drawing::Point(192, 248);
			this->s11A3->Name = S"s11A3";
			this->s11A3->Size = System::Drawing::Size(64, 20);
			this->s11A3->TabIndex = 121;
			this->s11A3->Text = S"XXXX PSIG";
			// 
			// s11A13
			// 
			this->s11A13->Enabled = false;
			this->s11A13->Location = System::Drawing::Point(192, 320);
			this->s11A13->Name = S"s11A13";
			this->s11A13->Size = System::Drawing::Size(64, 20);
			this->s11A13->TabIndex = 122;
			this->s11A13->Text = S"XXX.XX%";
			// 
			// s11A14
			// 
			this->s11A14->Enabled = false;
			this->s11A14->Location = System::Drawing::Point(456, 32);
			this->s11A14->Name = S"s11A14";
			this->s11A14->Size = System::Drawing::Size(64, 20);
			this->s11A14->TabIndex = 123;
			this->s11A14->Text = S"X.XXX PPH";
			// 
			// s11A55
			// 
			this->s11A55->Enabled = false;
			this->s11A55->Location = System::Drawing::Point(456, 56);
			this->s11A55->Name = S"s11A55";
			this->s11A55->Size = System::Drawing::Size(64, 20);
			this->s11A55->TabIndex = 124;
			this->s11A55->Text = S"XXXX PSIG";
			// 
			// s11A157
			// 
			this->s11A157->Enabled = false;
			this->s11A157->Location = System::Drawing::Point(456, 80);
			this->s11A157->Name = S"s11A157";
			this->s11A157->Size = System::Drawing::Size(64, 20);
			this->s11A157->TabIndex = 125;
			this->s11A157->Text = S"XXXX PSIG";
			// 
			// s11A129
			// 
			this->s11A129->Enabled = false;
			this->s11A129->Location = System::Drawing::Point(456, 128);
			this->s11A129->Name = S"s11A129";
			this->s11A129->Size = System::Drawing::Size(64, 20);
			this->s11A129->TabIndex = 126;
			this->s11A129->Text = S"XXX.XX%";
			// 
			// s10A8
			// 
			this->s10A8->Enabled = false;
			this->s10A8->Location = System::Drawing::Point(456, 152);
			this->s10A8->Name = S"s10A8";
			this->s10A8->Size = System::Drawing::Size(64, 20);
			this->s10A8->TabIndex = 127;
			this->s10A8->Text = S"XXXX PSIA";
			// 
			// s11A87
			// 
			this->s11A87->Enabled = false;
			this->s11A87->Location = System::Drawing::Point(456, 200);
			this->s11A87->Name = S"s11A87";
			this->s11A87->Size = System::Drawing::Size(64, 20);
			this->s11A87->TabIndex = 128;
			this->s11A87->Text = S"X.XXX PPH";
			// 
			// label29
			// 
			this->label29->Location = System::Drawing::Point(264, 320);
			this->label29->Name = S"label29";
			this->label29->Size = System::Drawing::Size(192, 24);
			this->label29->TabIndex = 129;
			this->label29->Text = S"RAD FLOW CONT SYS 1 OR 2";
			this->label29->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label30
			// 
			this->label30->Location = System::Drawing::Point(264, 344);
			this->label30->Name = S"label30";
			this->label30->Size = System::Drawing::Size(192, 24);
			this->label30->TabIndex = 130;
			this->label30->Text = S"H2O DUMP TEMP";
			this->label30->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label31
			// 
			this->label31->Location = System::Drawing::Point(8, 368);
			this->label31->Name = S"label31";
			this->label31->Size = System::Drawing::Size(176, 24);
			this->label31->TabIndex = 131;
			this->label31->Text = S"URINE DUMP NOZZLE TEMP";
			this->label31->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A135
			// 
			this->s10A135->Enabled = false;
			this->s10A135->Location = System::Drawing::Point(192, 368);
			this->s10A135->Name = S"s10A135";
			this->s10A135->Size = System::Drawing::Size(64, 20);
			this->s10A135->TabIndex = 133;
			this->s10A135->Text = S"XXX.XX °F";
			// 
			// s10A114
			// 
			this->s10A114->Enabled = false;
			this->s10A114->Location = System::Drawing::Point(456, 344);
			this->s10A114->Name = S"s10A114";
			this->s10A114->Size = System::Drawing::Size(64, 20);
			this->s10A114->TabIndex = 132;
			this->s10A114->Text = S"XXX.XX °F";
			// 
			// s11E4_8
			// 
			this->s11E4_8->Enabled = false;
			this->s11E4_8->Location = System::Drawing::Point(456, 320);
			this->s11E4_8->Name = S"s11E4_8";
			this->s11E4_8->Size = System::Drawing::Size(64, 20);
			this->s11E4_8->TabIndex = 134;
			this->s11E4_8->Text = S"XXXX";
			// 
			// ECSForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(526, 396);
			this->Controls->Add(this->s11E4_8);
			this->Controls->Add(this->s10A135);
			this->Controls->Add(this->s10A114);
			this->Controls->Add(this->label31);
			this->Controls->Add(this->label30);
			this->Controls->Add(this->label29);
			this->Controls->Add(this->s11A87);
			this->Controls->Add(this->s10A8);
			this->Controls->Add(this->s11A129);
			this->Controls->Add(this->s11A157);
			this->Controls->Add(this->s11A55);
			this->Controls->Add(this->s11A14);
			this->Controls->Add(this->s11A13);
			this->Controls->Add(this->s11A3);
			this->Controls->Add(this->s11A2);
			this->Controls->Add(this->s10A45);
			this->Controls->Add(this->s10A9);
			this->Controls->Add(this->s10A81);
			this->Controls->Add(this->s10A3);
			this->Controls->Add(this->s11A37);
			this->Controls->Add(this->s10A90);
			this->Controls->Add(this->s10A87);
			this->Controls->Add(this->s10A48);
			this->Controls->Add(this->s10A100);
			this->Controls->Add(this->s11A118);
			this->Controls->Add(this->s10A15);
			this->Controls->Add(this->s10A12);
			this->Controls->Add(this->s10A133);
			this->Controls->Add(this->s10A84);
			this->Controls->Add(this->s10A101);
			this->Controls->Add(this->s10A4);
			this->Controls->Add(this->s11A1);
			this->Controls->Add(this->s11A4);
			this->Controls->Add(this->label28);
			this->Controls->Add(this->label27);
			this->Controls->Add(this->label26);
			this->Controls->Add(this->label25);
			this->Controls->Add(this->label24);
			this->Controls->Add(this->label23);
			this->Controls->Add(this->label22);
			this->Controls->Add(this->label21);
			this->Controls->Add(this->label20);
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
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->s10A6);
			this->Controls->Add(this->label19);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = S"ECSForm";
			this->Text = S"ENVIRONMENTAL CONTROL SYSTEM";
			this->ResumeLayout(false);

		}		
	};
}