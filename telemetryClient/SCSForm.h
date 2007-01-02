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
	/// Summary for SCSForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	__gc class SCSForm : public System::Windows::Forms::Form
	{
	public: 
		SCSForm(void)
		{
			InitializeComponent();
			this->add_Closing(new CancelEventHandler(this,SCSForm_Closing));
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
	private: System::Void SCSForm_Closing(Object * sender, System::ComponentModel::CancelEventArgs* e){			
			e->Cancel = true; // don't close			
			this->Hide();     // Hide instead.
		}      		
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
	private: System::Windows::Forms::Label *  label19;
	private: System::Windows::Forms::Label *  label20;
	private: System::Windows::Forms::Label *  label21;
	private: System::Windows::Forms::Label *  label22;
	private: System::Windows::Forms::Label *  label23;
	private: System::Windows::Forms::Label *  label24;
	private: System::Windows::Forms::Label *  label25;
	private: System::Windows::Forms::Label *  label26;
	private: System::Windows::Forms::Label *  label27;
	private: System::Windows::Forms::Label *  label28;
	private: System::Windows::Forms::Label *  label29;
	private: System::Windows::Forms::Label *  label30;
	private: System::Windows::Forms::Label *  label31;
	private: System::Windows::Forms::Label *  label32;
	private: System::Windows::Forms::Label *  label33;
	private: System::Windows::Forms::Label *  label34;
	private: System::Windows::Forms::Label *  label35;
	private: System::Windows::Forms::Label *  label36;
	private: System::Windows::Forms::Label *  label37;
	private: System::Windows::Forms::Label *  label38;
	private: System::Windows::Forms::Label *  label39;
	private: System::Windows::Forms::Label *  label40;
	private: System::Windows::Forms::Label *  label41;
	private: System::Windows::Forms::Label *  label42;
	private: System::Windows::Forms::Label *  label43;
	private: System::Windows::Forms::Label *  label44;
	private: System::Windows::Forms::Label *  label45;
	private: System::Windows::Forms::Label *  label46;
	private: System::Windows::Forms::Label *  label47;
	private: System::Windows::Forms::Label *  label48;
	private: System::Windows::Forms::Label *  label49;
	private: System::Windows::Forms::Label *  label50;
	private: System::Windows::Forms::Label *  label51;
	private: System::Windows::Forms::Label *  label52;
	private: System::Windows::Forms::Label *  label53;
	private: System::Windows::Forms::Label *  label54;
	private: System::Windows::Forms::Label *  label55;
	private: System::Windows::Forms::Label *  label56;
	private: System::Windows::Forms::Label *  label57;
	private: System::Windows::Forms::Label *  label58;


























































private: System::Windows::Forms::TextBox *  s51A5;
private: System::Windows::Forms::TextBox *  s51A6;
private: System::Windows::Forms::TextBox *  s12A4;
private: System::Windows::Forms::TextBox *  s12A5;
private: System::Windows::Forms::TextBox *  s12A6;
private: System::Windows::Forms::TextBox *  s12A7;
private: System::Windows::Forms::TextBox *  s12A8;
private: System::Windows::Forms::TextBox *  s12A10;
private: System::Windows::Forms::TextBox *  s51A11;
private: System::Windows::Forms::TextBox *  s51A12;
private: System::Windows::Forms::TextBox *  s51A13;
private: System::Windows::Forms::TextBox *  s22E11;
private: System::Windows::Forms::TextBox *  s22E22;
private: System::Windows::Forms::TextBox *  s22E33;
private: System::Windows::Forms::TextBox *  s22E66;
private: System::Windows::Forms::TextBox *  s22E55;
private: System::Windows::Forms::TextBox *  s22E44;
private: System::Windows::Forms::TextBox *  s22E91;
private: System::Windows::Forms::TextBox *  s22E88;
private: System::Windows::Forms::TextBox *  s22E77;
private: System::Windows::Forms::TextBox *  s22E102;
private: System::Windows::Forms::TextBox *  s22E146;
private: System::Windows::Forms::TextBox *  s22E135;
private: System::Windows::Forms::TextBox *  s11E228;
private: System::Windows::Forms::TextBox *  s22E113;
private: System::Windows::Forms::TextBox *  s22E157;
private: System::Windows::Forms::TextBox *  s11E242;
private: System::Windows::Forms::TextBox *  s11E238;
private: System::Windows::Forms::TextBox *  s11E236;
private: System::Windows::Forms::TextBox *  s12A13;
private: System::Windows::Forms::TextBox *  s12A15;
private: System::Windows::Forms::TextBox *  s11E243;
private: System::Windows::Forms::TextBox *  s11E93;
private: System::Windows::Forms::TextBox *  s11E86;
private: System::Windows::Forms::TextBox *  s11E85;
private: System::Windows::Forms::TextBox *  s11E84;
private: System::Windows::Forms::TextBox *  s11E82;
private: System::Windows::Forms::TextBox *  s11E94;
private: System::Windows::Forms::TextBox *  s11E44;
private: System::Windows::Forms::TextBox *  s11E244;
private: System::Windows::Forms::TextBox *  s11E257;
private: System::Windows::Forms::TextBox *  s11E95;
private: System::Windows::Forms::TextBox *  s11E251;
private: System::Windows::Forms::TextBox *  s11E254;
private: System::Windows::Forms::TextBox *  s11E253;
private: System::Windows::Forms::TextBox *  s11E258;
private: System::Windows::Forms::TextBox *  s11E87;
private: System::Windows::Forms::TextBox *  s11E256;
private: System::Windows::Forms::TextBox *  s11E255;
private: System::Windows::Forms::TextBox *  s11E332;
private: System::Windows::Forms::TextBox *  s11E331;
private: System::Windows::Forms::TextBox *  s11E88;
private: System::Windows::Forms::TextBox *  s11E334;
private: System::Windows::Forms::TextBox *  s11E333;
private: System::Windows::Forms::TextBox *  s11E335;
private: System::Windows::Forms::TextBox *  s11E336;
private: System::Windows::Forms::TextBox *  s11E224;
private: System::Windows::Forms::TextBox *  s22E168;
private: System::Windows::Forms::TextBox *  s22E124;
private: System::Windows::Forms::TextBox *  s11E226;
private: System::Windows::Forms::Label *  label59;
private: System::Windows::Forms::Label *  label60;
private: System::Windows::Forms::Label *  label61;
private: System::Windows::Forms::Label *  label62;
private: System::Windows::Forms::TextBox *  s22A4;
private: System::Windows::Forms::TextBox *  s12A16;

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
			this->label19 = new System::Windows::Forms::Label();
			this->label20 = new System::Windows::Forms::Label();
			this->label21 = new System::Windows::Forms::Label();
			this->label22 = new System::Windows::Forms::Label();
			this->label23 = new System::Windows::Forms::Label();
			this->label24 = new System::Windows::Forms::Label();
			this->label25 = new System::Windows::Forms::Label();
			this->label26 = new System::Windows::Forms::Label();
			this->label27 = new System::Windows::Forms::Label();
			this->label28 = new System::Windows::Forms::Label();
			this->label29 = new System::Windows::Forms::Label();
			this->label30 = new System::Windows::Forms::Label();
			this->label31 = new System::Windows::Forms::Label();
			this->label32 = new System::Windows::Forms::Label();
			this->label33 = new System::Windows::Forms::Label();
			this->label34 = new System::Windows::Forms::Label();
			this->label35 = new System::Windows::Forms::Label();
			this->label36 = new System::Windows::Forms::Label();
			this->label37 = new System::Windows::Forms::Label();
			this->label38 = new System::Windows::Forms::Label();
			this->label39 = new System::Windows::Forms::Label();
			this->label40 = new System::Windows::Forms::Label();
			this->label41 = new System::Windows::Forms::Label();
			this->label42 = new System::Windows::Forms::Label();
			this->label43 = new System::Windows::Forms::Label();
			this->label44 = new System::Windows::Forms::Label();
			this->label45 = new System::Windows::Forms::Label();
			this->label46 = new System::Windows::Forms::Label();
			this->label47 = new System::Windows::Forms::Label();
			this->label48 = new System::Windows::Forms::Label();
			this->label49 = new System::Windows::Forms::Label();
			this->label50 = new System::Windows::Forms::Label();
			this->label51 = new System::Windows::Forms::Label();
			this->label52 = new System::Windows::Forms::Label();
			this->label53 = new System::Windows::Forms::Label();
			this->label54 = new System::Windows::Forms::Label();
			this->label55 = new System::Windows::Forms::Label();
			this->label56 = new System::Windows::Forms::Label();
			this->label57 = new System::Windows::Forms::Label();
			this->label58 = new System::Windows::Forms::Label();
			this->s51A5 = new System::Windows::Forms::TextBox();
			this->s51A6 = new System::Windows::Forms::TextBox();
			this->s12A4 = new System::Windows::Forms::TextBox();
			this->s12A5 = new System::Windows::Forms::TextBox();
			this->s12A6 = new System::Windows::Forms::TextBox();
			this->s12A7 = new System::Windows::Forms::TextBox();
			this->s12A8 = new System::Windows::Forms::TextBox();
			this->s12A10 = new System::Windows::Forms::TextBox();
			this->s51A11 = new System::Windows::Forms::TextBox();
			this->s51A12 = new System::Windows::Forms::TextBox();
			this->s51A13 = new System::Windows::Forms::TextBox();
			this->s22E11 = new System::Windows::Forms::TextBox();
			this->s22E22 = new System::Windows::Forms::TextBox();
			this->s22E33 = new System::Windows::Forms::TextBox();
			this->s22E66 = new System::Windows::Forms::TextBox();
			this->s22E55 = new System::Windows::Forms::TextBox();
			this->s22E44 = new System::Windows::Forms::TextBox();
			this->s22E91 = new System::Windows::Forms::TextBox();
			this->s22E88 = new System::Windows::Forms::TextBox();
			this->s22E77 = new System::Windows::Forms::TextBox();
			this->s22E102 = new System::Windows::Forms::TextBox();
			this->s22E146 = new System::Windows::Forms::TextBox();
			this->s22E135 = new System::Windows::Forms::TextBox();
			this->s11E228 = new System::Windows::Forms::TextBox();
			this->s22E113 = new System::Windows::Forms::TextBox();
			this->s22E157 = new System::Windows::Forms::TextBox();
			this->s11E242 = new System::Windows::Forms::TextBox();
			this->s11E238 = new System::Windows::Forms::TextBox();
			this->s11E236 = new System::Windows::Forms::TextBox();
			this->s12A13 = new System::Windows::Forms::TextBox();
			this->s12A15 = new System::Windows::Forms::TextBox();
			this->s11E243 = new System::Windows::Forms::TextBox();
			this->s11E93 = new System::Windows::Forms::TextBox();
			this->s11E86 = new System::Windows::Forms::TextBox();
			this->s11E85 = new System::Windows::Forms::TextBox();
			this->s11E84 = new System::Windows::Forms::TextBox();
			this->s11E82 = new System::Windows::Forms::TextBox();
			this->s11E94 = new System::Windows::Forms::TextBox();
			this->s11E44 = new System::Windows::Forms::TextBox();
			this->s11E244 = new System::Windows::Forms::TextBox();
			this->s11E257 = new System::Windows::Forms::TextBox();
			this->s11E95 = new System::Windows::Forms::TextBox();
			this->s11E251 = new System::Windows::Forms::TextBox();
			this->s11E254 = new System::Windows::Forms::TextBox();
			this->s11E253 = new System::Windows::Forms::TextBox();
			this->s11E258 = new System::Windows::Forms::TextBox();
			this->s11E87 = new System::Windows::Forms::TextBox();
			this->s11E256 = new System::Windows::Forms::TextBox();
			this->s11E255 = new System::Windows::Forms::TextBox();
			this->s11E332 = new System::Windows::Forms::TextBox();
			this->s11E331 = new System::Windows::Forms::TextBox();
			this->s11E88 = new System::Windows::Forms::TextBox();
			this->s11E334 = new System::Windows::Forms::TextBox();
			this->s11E333 = new System::Windows::Forms::TextBox();
			this->s11E335 = new System::Windows::Forms::TextBox();
			this->s11E336 = new System::Windows::Forms::TextBox();
			this->s22A4 = new System::Windows::Forms::TextBox();
			this->s12A16 = new System::Windows::Forms::TextBox();
			this->s11E224 = new System::Windows::Forms::TextBox();
			this->s22E168 = new System::Windows::Forms::TextBox();
			this->s22E124 = new System::Windows::Forms::TextBox();
			this->s11E226 = new System::Windows::Forms::TextBox();
			this->label59 = new System::Windows::Forms::Label();
			this->label60 = new System::Windows::Forms::Label();
			this->label61 = new System::Windows::Forms::Label();
			this->label62 = new System::Windows::Forms::Label();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 8);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(144, 24);
			this->label1->TabIndex = 4;
			this->label1->Text = S"PITCH ATT ERR";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 32);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(144, 24);
			this->label2->TabIndex = 5;
			this->label2->Text = S"YAW ATT ERR";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 56);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(144, 24);
			this->label3->TabIndex = 6;
			this->label3->Text = S"ROLL ATT ERR";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(8, 80);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(144, 24);
			this->label4->TabIndex = 7;
			this->label4->Text = S"SCS PITCH BODY RATE";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(8, 104);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(144, 24);
			this->label5->TabIndex = 8;
			this->label5->Text = S"SCS YAW BODY RATE";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(8, 128);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(144, 24);
			this->label6->TabIndex = 9;
			this->label6->Text = S"SCS ROLL BODY RATE";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(8, 152);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(144, 24);
			this->label7->TabIndex = 10;
			this->label7->Text = S"PITCH GIMBL POS 1 OR 2";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(8, 176);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(144, 24);
			this->label8->TabIndex = 11;
			this->label8->Text = S"YAW GIMBL POS 1 OR 2";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(8, 200);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(144, 24);
			this->label9->TabIndex = 12;
			this->label9->Text = S"+PITCH/+X ENG";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 224);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(144, 24);
			this->label10->TabIndex = 13;
			this->label10->Text = S"-PITCH/+X ENG";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(8, 248);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(144, 24);
			this->label11->TabIndex = 14;
			this->label11->Text = S"+PITCH/-X ENG";
			this->label11->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label12
			// 
			this->label12->Location = System::Drawing::Point(8, 272);
			this->label12->Name = S"label12";
			this->label12->Size = System::Drawing::Size(144, 24);
			this->label12->TabIndex = 15;
			this->label12->Text = S"-PITCH/-X ENG";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label13
			// 
			this->label13->Location = System::Drawing::Point(8, 368);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(144, 24);
			this->label13->TabIndex = 19;
			this->label13->Text = S"-YAW/-X ENG";
			this->label13->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label14
			// 
			this->label14->Location = System::Drawing::Point(8, 344);
			this->label14->Name = S"label14";
			this->label14->Size = System::Drawing::Size(144, 24);
			this->label14->TabIndex = 18;
			this->label14->Text = S"+YAW/-X ENG";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label15
			// 
			this->label15->Location = System::Drawing::Point(8, 320);
			this->label15->Name = S"label15";
			this->label15->Size = System::Drawing::Size(144, 24);
			this->label15->TabIndex = 17;
			this->label15->Text = S"-YAW/+X ENG";
			this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label16
			// 
			this->label16->Location = System::Drawing::Point(8, 296);
			this->label16->Name = S"label16";
			this->label16->Size = System::Drawing::Size(144, 24);
			this->label16->TabIndex = 16;
			this->label16->Text = S"+YAW/+X ENG";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label17
			// 
			this->label17->Location = System::Drawing::Point(8, 464);
			this->label17->Name = S"label17";
			this->label17->Size = System::Drawing::Size(144, 24);
			this->label17->TabIndex = 23;
			this->label17->Text = S"-ROLL/-Z ENG";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label18
			// 
			this->label18->Location = System::Drawing::Point(8, 440);
			this->label18->Name = S"label18";
			this->label18->Size = System::Drawing::Size(144, 24);
			this->label18->TabIndex = 22;
			this->label18->Text = S"+ROLL/-Z ENG";
			this->label18->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label19
			// 
			this->label19->Location = System::Drawing::Point(8, 416);
			this->label19->Name = S"label19";
			this->label19->Size = System::Drawing::Size(144, 24);
			this->label19->TabIndex = 21;
			this->label19->Text = S"-ROLL/+Z ENG";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label20
			// 
			this->label20->Location = System::Drawing::Point(8, 392);
			this->label20->Name = S"label20";
			this->label20->Size = System::Drawing::Size(144, 24);
			this->label20->TabIndex = 20;
			this->label20->Text = S"+ROLL/+Z ENG";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label21
			// 
			this->label21->Location = System::Drawing::Point(224, 104);
			this->label21->Name = S"label21";
			this->label21->Size = System::Drawing::Size(160, 24);
			this->label21->TabIndex = 24;
			this->label21->Text = S"TRANS CTL +X CMD";
			this->label21->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label22
			// 
			this->label22->Location = System::Drawing::Point(224, 128);
			this->label22->Name = S"label22";
			this->label22->Size = System::Drawing::Size(160, 24);
			this->label22->TabIndex = 25;
			this->label22->Text = S"TRANS CTL -X CMD";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label23
			// 
			this->label23->Location = System::Drawing::Point(224, 176);
			this->label23->Name = S"label23";
			this->label23->Size = System::Drawing::Size(160, 24);
			this->label23->TabIndex = 27;
			this->label23->Text = S"TRANS CTL -Y CMD";
			this->label23->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label24
			// 
			this->label24->Location = System::Drawing::Point(224, 152);
			this->label24->Name = S"label24";
			this->label24->Size = System::Drawing::Size(160, 24);
			this->label24->TabIndex = 26;
			this->label24->Text = S"TRANS CTL +Y CMD";
			this->label24->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label25
			// 
			this->label25->Location = System::Drawing::Point(224, 224);
			this->label25->Name = S"label25";
			this->label25->Size = System::Drawing::Size(160, 24);
			this->label25->TabIndex = 29;
			this->label25->Text = S"TRANS CTL -Z CMD";
			this->label25->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label26
			// 
			this->label26->Location = System::Drawing::Point(224, 200);
			this->label26->Name = S"label26";
			this->label26->Size = System::Drawing::Size(160, 24);
			this->label26->TabIndex = 28;
			this->label26->Text = S"TRANS CTL +Z CMD";
			this->label26->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label27
			// 
			this->label27->Location = System::Drawing::Point(224, 248);
			this->label27->Name = S"label27";
			this->label27->Size = System::Drawing::Size(160, 24);
			this->label27->TabIndex = 30;
			this->label27->Text = S"SCS TVC PITCH AUTO CMD";
			this->label27->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label28
			// 
			this->label28->Location = System::Drawing::Point(224, 272);
			this->label28->Name = S"label28";
			this->label28->Size = System::Drawing::Size(160, 24);
			this->label28->TabIndex = 31;
			this->label28->Text = S"SCS TVC YAW AUTO CMD";
			this->label28->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label29
			// 
			this->label29->Location = System::Drawing::Point(224, 320);
			this->label29->Name = S"label29";
			this->label29->Size = System::Drawing::Size(160, 24);
			this->label29->TabIndex = 33;
			this->label29->Text = S"MTVC YAW CMD";
			this->label29->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label30
			// 
			this->label30->Location = System::Drawing::Point(224, 296);
			this->label30->Name = S"label30";
			this->label30->Size = System::Drawing::Size(160, 24);
			this->label30->TabIndex = 32;
			this->label30->Text = S"MTVC PITCH CMD";
			this->label30->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label31
			// 
			this->label31->Location = System::Drawing::Point(224, 344);
			this->label31->Name = S"label31";
			this->label31->Size = System::Drawing::Size(160, 24);
			this->label31->TabIndex = 34;
			this->label31->Text = S"ROT ROLL CMD";
			this->label31->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label32
			// 
			this->label32->Location = System::Drawing::Point(224, 368);
			this->label32->Name = S"label32";
			this->label32->Size = System::Drawing::Size(160, 24);
			this->label32->TabIndex = 35;
			this->label32->Text = S"ATT DEADBAND MIN";
			this->label32->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label33
			// 
			this->label33->Location = System::Drawing::Point(224, 392);
			this->label33->Name = S"label33";
			this->label33->Size = System::Drawing::Size(160, 24);
			this->label33->TabIndex = 36;
			this->label33->Text = S"HI RATE LIMIT";
			this->label33->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label34
			// 
			this->label34->Location = System::Drawing::Point(224, 416);
			this->label34->Name = S"label34";
			this->label34->Size = System::Drawing::Size(160, 24);
			this->label34->TabIndex = 37;
			this->label34->Text = S"FDAI ERR 5 RATE 5 ";
			this->label34->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label35
			// 
			this->label35->Location = System::Drawing::Point(224, 440);
			this->label35->Name = S"label35";
			this->label35->Size = System::Drawing::Size(160, 24);
			this->label35->TabIndex = 38;
			this->label35->Text = S"FDAI SCALE ERR 50/15";
			this->label35->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label36
			// 
			this->label36->Location = System::Drawing::Point(224, 464);
			this->label36->Name = S"label36";
			this->label36->Size = System::Drawing::Size(160, 24);
			this->label36->TabIndex = 39;
			this->label36->Text = S"dV CG LM/CSM";
			this->label36->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label37
			// 
			this->label37->Location = System::Drawing::Point(456, 8);
			this->label37->Name = S"label37";
			this->label37->Size = System::Drawing::Size(160, 24);
			this->label37->TabIndex = 40;
			this->label37->Text = S"DIRECT RCS NO 1";
			this->label37->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label38
			// 
			this->label38->Location = System::Drawing::Point(456, 32);
			this->label38->Name = S"label38";
			this->label38->Size = System::Drawing::Size(160, 24);
			this->label38->TabIndex = 41;
			this->label38->Text = S"DIRECT RCS NO 2";
			this->label38->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label39
			// 
			this->label39->Location = System::Drawing::Point(456, 56);
			this->label39->Name = S"label39";
			this->label39->Size = System::Drawing::Size(160, 24);
			this->label39->TabIndex = 42;
			this->label39->Text = S"SPS SOL DRIVER 1";
			this->label39->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label40
			// 
			this->label40->Location = System::Drawing::Point(456, 80);
			this->label40->Name = S"label40";
			this->label40->Size = System::Drawing::Size(160, 24);
			this->label40->TabIndex = 43;
			this->label40->Text = S"SPS SOL DRIVER 2";
			this->label40->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label41
			// 
			this->label41->Location = System::Drawing::Point(456, 104);
			this->label41->Name = S"label41";
			this->label41->Size = System::Drawing::Size(200, 24);
			this->label41->TabIndex = 44;
			this->label41->Text = S"LIMIT CYCLE SWITCH";
			this->label41->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label42
			// 
			this->label42->Location = System::Drawing::Point(456, 128);
			this->label42->Name = S"label42";
			this->label42->Size = System::Drawing::Size(200, 24);
			this->label42->TabIndex = 45;
			this->label42->Text = S"S/C CNTL SCS";
			this->label42->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label43
			// 
			this->label43->Location = System::Drawing::Point(456, 152);
			this->label43->Name = S"label43";
			this->label43->Size = System::Drawing::Size(200, 24);
			this->label43->TabIndex = 46;
			this->label43->Text = S"MANUAL ATT ROLL ACCEL CMD";
			this->label43->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label44
			// 
			this->label44->Location = System::Drawing::Point(456, 176);
			this->label44->Name = S"label44";
			this->label44->Size = System::Drawing::Size(200, 24);
			this->label44->TabIndex = 47;
			this->label44->Text = S"MANUAL ATT ROLL MIN IMP CMD";
			this->label44->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label45
			// 
			this->label45->Location = System::Drawing::Point(456, 224);
			this->label45->Name = S"label45";
			this->label45->Size = System::Drawing::Size(200, 24);
			this->label45->TabIndex = 49;
			this->label45->Text = S"MANUAL ATT PITCH MIN IMP CMD";
			this->label45->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label46
			// 
			this->label46->Location = System::Drawing::Point(456, 200);
			this->label46->Name = S"label46";
			this->label46->Size = System::Drawing::Size(200, 24);
			this->label46->TabIndex = 48;
			this->label46->Text = S"MANUAL ATT PITCH ACCEL CMD";
			this->label46->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label47
			// 
			this->label47->Location = System::Drawing::Point(456, 272);
			this->label47->Name = S"label47";
			this->label47->Size = System::Drawing::Size(200, 24);
			this->label47->TabIndex = 51;
			this->label47->Text = S"MANUAL ATT YAW MIN IMP CMD";
			this->label47->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label48
			// 
			this->label48->Location = System::Drawing::Point(456, 248);
			this->label48->Name = S"label48";
			this->label48->Size = System::Drawing::Size(200, 24);
			this->label48->TabIndex = 50;
			this->label48->Text = S"MANUAL ATT ROLL YAW CMD";
			this->label48->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label49
			// 
			this->label49->Location = System::Drawing::Point(456, 296);
			this->label49->Name = S"label49";
			this->label49->Size = System::Drawing::Size(200, 24);
			this->label49->TabIndex = 52;
			this->label49->Text = S"GYRO 1 COMB SMRD";
			this->label49->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label50
			// 
			this->label50->Location = System::Drawing::Point(456, 320);
			this->label50->Name = S"label50";
			this->label50->Size = System::Drawing::Size(200, 24);
			this->label50->TabIndex = 53;
			this->label50->Text = S"GYRO 2 COMB SMRD";
			this->label50->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label51
			// 
			this->label51->Location = System::Drawing::Point(456, 344);
			this->label51->Name = S"label51";
			this->label51->Size = System::Drawing::Size(200, 24);
			this->label51->TabIndex = 54;
			this->label51->Text = S"BMAG MODE SW-ROLL ATT 1 RT 2";
			this->label51->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label52
			// 
			this->label52->Location = System::Drawing::Point(456, 368);
			this->label52->Name = S"label52";
			this->label52->Size = System::Drawing::Size(200, 24);
			this->label52->TabIndex = 55;
			this->label52->Text = S"BMAG MODE SW-ROLL RATE 2";
			this->label52->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label53
			// 
			this->label53->Location = System::Drawing::Point(456, 416);
			this->label53->Name = S"label53";
			this->label53->Size = System::Drawing::Size(200, 24);
			this->label53->TabIndex = 57;
			this->label53->Text = S"BMAG MODE SW-PITCH RATE 2";
			this->label53->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label54
			// 
			this->label54->Location = System::Drawing::Point(456, 392);
			this->label54->Name = S"label54";
			this->label54->Size = System::Drawing::Size(200, 24);
			this->label54->TabIndex = 56;
			this->label54->Text = S"BMAG MODE SW-PITCH ATT 1 RT 2";
			this->label54->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label55
			// 
			this->label55->Location = System::Drawing::Point(456, 464);
			this->label55->Name = S"label55";
			this->label55->Size = System::Drawing::Size(200, 24);
			this->label55->TabIndex = 59;
			this->label55->Text = S"BMAG MODE SW-YAW RATE 2";
			this->label55->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label56
			// 
			this->label56->Location = System::Drawing::Point(456, 440);
			this->label56->Name = S"label56";
			this->label56->Size = System::Drawing::Size(200, 24);
			this->label56->TabIndex = 58;
			this->label56->Text = S"BMAG MODE SW-YAW ATT 1 RT 2";
			this->label56->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label57
			// 
			this->label57->Location = System::Drawing::Point(88, 488);
			this->label57->Name = S"label57";
			this->label57->Size = System::Drawing::Size(200, 24);
			this->label57->TabIndex = 60;
			this->label57->Text = S"PITCH DIFF CLUTCH CURRENT";
			this->label57->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label58
			// 
			this->label58->Location = System::Drawing::Point(360, 488);
			this->label58->Name = S"label58";
			this->label58->Size = System::Drawing::Size(200, 24);
			this->label58->TabIndex = 61;
			this->label58->Text = S"YAW DIFF CLUTCH CURRENT";
			this->label58->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s51A5
			// 
			this->s51A5->Enabled = false;
			this->s51A5->Location = System::Drawing::Point(152, 8);
			this->s51A5->Name = S"s51A5";
			this->s51A5->Size = System::Drawing::Size(64, 20);
			this->s51A5->TabIndex = 62;
			this->s51A5->Text = S"XXX.XX °";
			// 
			// s51A6
			// 
			this->s51A6->Enabled = false;
			this->s51A6->Location = System::Drawing::Point(152, 32);
			this->s51A6->Name = S"s51A6";
			this->s51A6->Size = System::Drawing::Size(64, 20);
			this->s51A6->TabIndex = 63;
			this->s51A6->Text = S"XXX.XX °";
			// 
			// s12A4
			// 
			this->s12A4->Enabled = false;
			this->s12A4->Location = System::Drawing::Point(152, 56);
			this->s12A4->Name = S"s12A4";
			this->s12A4->Size = System::Drawing::Size(64, 20);
			this->s12A4->TabIndex = 64;
			this->s12A4->Text = S"XXX.XX °";
			// 
			// s12A5
			// 
			this->s12A5->Enabled = false;
			this->s12A5->Location = System::Drawing::Point(152, 80);
			this->s12A5->Name = S"s12A5";
			this->s12A5->Size = System::Drawing::Size(64, 20);
			this->s12A5->TabIndex = 65;
			this->s12A5->Text = S"XXX.XX °";
			// 
			// s12A6
			// 
			this->s12A6->Enabled = false;
			this->s12A6->Location = System::Drawing::Point(152, 104);
			this->s12A6->Name = S"s12A6";
			this->s12A6->Size = System::Drawing::Size(64, 20);
			this->s12A6->TabIndex = 66;
			this->s12A6->Text = S"XXX.XX °";
			// 
			// s12A7
			// 
			this->s12A7->Enabled = false;
			this->s12A7->Location = System::Drawing::Point(152, 128);
			this->s12A7->Name = S"s12A7";
			this->s12A7->Size = System::Drawing::Size(64, 20);
			this->s12A7->TabIndex = 67;
			this->s12A7->Text = S"XXX.XX °";
			// 
			// s12A8
			// 
			this->s12A8->Enabled = false;
			this->s12A8->Location = System::Drawing::Point(152, 152);
			this->s12A8->Name = S"s12A8";
			this->s12A8->Size = System::Drawing::Size(64, 20);
			this->s12A8->TabIndex = 68;
			this->s12A8->Text = S"XXX.XX °";
			// 
			// s12A10
			// 
			this->s12A10->Enabled = false;
			this->s12A10->Location = System::Drawing::Point(152, 176);
			this->s12A10->Name = S"s12A10";
			this->s12A10->Size = System::Drawing::Size(64, 20);
			this->s12A10->TabIndex = 69;
			this->s12A10->Text = S"XXX.XX °";
			// 
			// s51A11
			// 
			this->s51A11->Enabled = false;
			this->s51A11->Location = System::Drawing::Point(384, 296);
			this->s51A11->Name = S"s51A11";
			this->s51A11->Size = System::Drawing::Size(64, 20);
			this->s51A11->TabIndex = 70;
			this->s51A11->Text = S"XXX.XX °";
			// 
			// s51A12
			// 
			this->s51A12->Enabled = false;
			this->s51A12->Location = System::Drawing::Point(384, 320);
			this->s51A12->Name = S"s51A12";
			this->s51A12->Size = System::Drawing::Size(64, 20);
			this->s51A12->TabIndex = 71;
			this->s51A12->Text = S"XXX.XX °";
			// 
			// s51A13
			// 
			this->s51A13->Enabled = false;
			this->s51A13->Location = System::Drawing::Point(384, 344);
			this->s51A13->Name = S"s51A13";
			this->s51A13->Size = System::Drawing::Size(64, 20);
			this->s51A13->TabIndex = 72;
			this->s51A13->Text = S"XXX.XX °";
			// 
			// s22E11
			// 
			this->s22E11->Enabled = false;
			this->s22E11->Location = System::Drawing::Point(152, 200);
			this->s22E11->Name = S"s22E11";
			this->s22E11->Size = System::Drawing::Size(64, 20);
			this->s22E11->TabIndex = 101;
			this->s22E11->Text = S"XXXX";
			// 
			// s22E22
			// 
			this->s22E22->Enabled = false;
			this->s22E22->Location = System::Drawing::Point(152, 224);
			this->s22E22->Name = S"s22E22";
			this->s22E22->Size = System::Drawing::Size(64, 20);
			this->s22E22->TabIndex = 102;
			this->s22E22->Text = S"XXXX";
			// 
			// s22E33
			// 
			this->s22E33->Enabled = false;
			this->s22E33->Location = System::Drawing::Point(152, 248);
			this->s22E33->Name = S"s22E33";
			this->s22E33->Size = System::Drawing::Size(64, 20);
			this->s22E33->TabIndex = 103;
			this->s22E33->Text = S"XXXX";
			// 
			// s22E66
			// 
			this->s22E66->Enabled = false;
			this->s22E66->Location = System::Drawing::Point(152, 320);
			this->s22E66->Name = S"s22E66";
			this->s22E66->Size = System::Drawing::Size(64, 20);
			this->s22E66->TabIndex = 106;
			this->s22E66->Text = S"XXXX";
			// 
			// s22E55
			// 
			this->s22E55->Enabled = false;
			this->s22E55->Location = System::Drawing::Point(152, 296);
			this->s22E55->Name = S"s22E55";
			this->s22E55->Size = System::Drawing::Size(64, 20);
			this->s22E55->TabIndex = 105;
			this->s22E55->Text = S"XXXX";
			// 
			// s22E44
			// 
			this->s22E44->Enabled = false;
			this->s22E44->Location = System::Drawing::Point(152, 272);
			this->s22E44->Name = S"s22E44";
			this->s22E44->Size = System::Drawing::Size(64, 20);
			this->s22E44->TabIndex = 104;
			this->s22E44->Text = S"XXXX";
			// 
			// s22E91
			// 
			this->s22E91->Enabled = false;
			this->s22E91->Location = System::Drawing::Point(152, 392);
			this->s22E91->Name = S"s22E91";
			this->s22E91->Size = System::Drawing::Size(64, 20);
			this->s22E91->TabIndex = 109;
			this->s22E91->Text = S"XXXX";
			// 
			// s22E88
			// 
			this->s22E88->Enabled = false;
			this->s22E88->Location = System::Drawing::Point(152, 368);
			this->s22E88->Name = S"s22E88";
			this->s22E88->Size = System::Drawing::Size(64, 20);
			this->s22E88->TabIndex = 108;
			this->s22E88->Text = S"XXXX";
			// 
			// s22E77
			// 
			this->s22E77->Enabled = false;
			this->s22E77->Location = System::Drawing::Point(152, 344);
			this->s22E77->Name = S"s22E77";
			this->s22E77->Size = System::Drawing::Size(64, 20);
			this->s22E77->TabIndex = 107;
			this->s22E77->Text = S"XXXX";
			// 
			// s22E102
			// 
			this->s22E102->Enabled = false;
			this->s22E102->Location = System::Drawing::Point(152, 464);
			this->s22E102->Name = S"s22E102";
			this->s22E102->Size = System::Drawing::Size(64, 20);
			this->s22E102->TabIndex = 112;
			this->s22E102->Text = S"XXXX";
			// 
			// s22E146
			// 
			this->s22E146->Enabled = false;
			this->s22E146->Location = System::Drawing::Point(152, 440);
			this->s22E146->Name = S"s22E146";
			this->s22E146->Size = System::Drawing::Size(64, 20);
			this->s22E146->TabIndex = 111;
			this->s22E146->Text = S"XXXX";
			// 
			// s22E135
			// 
			this->s22E135->Enabled = false;
			this->s22E135->Location = System::Drawing::Point(152, 416);
			this->s22E135->Name = S"s22E135";
			this->s22E135->Size = System::Drawing::Size(64, 20);
			this->s22E135->TabIndex = 110;
			this->s22E135->Text = S"XXXX";
			// 
			// s11E228
			// 
			this->s11E228->Enabled = false;
			this->s11E228->Location = System::Drawing::Point(384, 152);
			this->s11E228->Name = S"s11E228";
			this->s11E228->Size = System::Drawing::Size(64, 20);
			this->s11E228->TabIndex = 115;
			this->s11E228->Text = S"XXXX";
			// 
			// s22E113
			// 
			this->s22E113->Enabled = false;
			this->s22E113->Location = System::Drawing::Point(384, 32);
			this->s22E113->Name = S"s22E113";
			this->s22E113->Size = System::Drawing::Size(64, 20);
			this->s22E113->TabIndex = 114;
			this->s22E113->Text = S"XXXX";
			// 
			// s22E157
			// 
			this->s22E157->Enabled = false;
			this->s22E157->Location = System::Drawing::Point(384, 8);
			this->s22E157->Name = S"s22E157";
			this->s22E157->Size = System::Drawing::Size(64, 20);
			this->s22E157->TabIndex = 113;
			this->s22E157->Text = S"XXXX";
			// 
			// s11E242
			// 
			this->s11E242->Enabled = false;
			this->s11E242->Location = System::Drawing::Point(384, 224);
			this->s11E242->Name = S"s11E242";
			this->s11E242->Size = System::Drawing::Size(64, 20);
			this->s11E242->TabIndex = 118;
			this->s11E242->Text = S"XXXX";
			// 
			// s11E238
			// 
			this->s11E238->Enabled = false;
			this->s11E238->Location = System::Drawing::Point(384, 200);
			this->s11E238->Name = S"s11E238";
			this->s11E238->Size = System::Drawing::Size(64, 20);
			this->s11E238->TabIndex = 117;
			this->s11E238->Text = S"XXXX";
			// 
			// s11E236
			// 
			this->s11E236->Enabled = false;
			this->s11E236->Location = System::Drawing::Point(384, 176);
			this->s11E236->Name = S"s11E236";
			this->s11E236->Size = System::Drawing::Size(64, 20);
			this->s11E236->TabIndex = 116;
			this->s11E236->Text = S"XXXX";
			// 
			// s12A13
			// 
			this->s12A13->Enabled = false;
			this->s12A13->Location = System::Drawing::Point(384, 272);
			this->s12A13->Name = S"s12A13";
			this->s12A13->Size = System::Drawing::Size(64, 20);
			this->s12A13->TabIndex = 120;
			this->s12A13->Text = S"XX.XX V";
			// 
			// s12A15
			// 
			this->s12A15->Enabled = false;
			this->s12A15->Location = System::Drawing::Point(384, 248);
			this->s12A15->Name = S"s12A15";
			this->s12A15->Size = System::Drawing::Size(64, 20);
			this->s12A15->TabIndex = 119;
			this->s12A15->Text = S"XX.XX V";
			// 
			// s11E243
			// 
			this->s11E243->Enabled = false;
			this->s11E243->Location = System::Drawing::Point(656, 8);
			this->s11E243->Name = S"s11E243";
			this->s11E243->Size = System::Drawing::Size(64, 20);
			this->s11E243->TabIndex = 126;
			this->s11E243->Text = S"XXXX";
			// 
			// s11E93
			// 
			this->s11E93->Enabled = false;
			this->s11E93->Location = System::Drawing::Point(384, 464);
			this->s11E93->Name = S"s11E93";
			this->s11E93->Size = System::Drawing::Size(64, 20);
			this->s11E93->TabIndex = 125;
			this->s11E93->Text = S"XXXX";
			// 
			// s11E86
			// 
			this->s11E86->Enabled = false;
			this->s11E86->Location = System::Drawing::Point(384, 440);
			this->s11E86->Name = S"s11E86";
			this->s11E86->Size = System::Drawing::Size(64, 20);
			this->s11E86->TabIndex = 124;
			this->s11E86->Text = S"XXXX";
			// 
			// s11E85
			// 
			this->s11E85->Enabled = false;
			this->s11E85->Location = System::Drawing::Point(384, 416);
			this->s11E85->Name = S"s11E85";
			this->s11E85->Size = System::Drawing::Size(64, 20);
			this->s11E85->TabIndex = 123;
			this->s11E85->Text = S"XXXX";
			// 
			// s11E84
			// 
			this->s11E84->Enabled = false;
			this->s11E84->Location = System::Drawing::Point(384, 392);
			this->s11E84->Name = S"s11E84";
			this->s11E84->Size = System::Drawing::Size(64, 20);
			this->s11E84->TabIndex = 122;
			this->s11E84->Text = S"XXXX";
			// 
			// s11E82
			// 
			this->s11E82->Enabled = false;
			this->s11E82->Location = System::Drawing::Point(384, 368);
			this->s11E82->Name = S"s11E82";
			this->s11E82->Size = System::Drawing::Size(64, 20);
			this->s11E82->TabIndex = 121;
			this->s11E82->Text = S"XXXX";
			// 
			// s11E94
			// 
			this->s11E94->Enabled = false;
			this->s11E94->Location = System::Drawing::Point(656, 80);
			this->s11E94->Name = S"s11E94";
			this->s11E94->Size = System::Drawing::Size(64, 20);
			this->s11E94->TabIndex = 129;
			this->s11E94->Text = S"XXXX";
			// 
			// s11E44
			// 
			this->s11E44->Enabled = false;
			this->s11E44->Location = System::Drawing::Point(656, 56);
			this->s11E44->Name = S"s11E44";
			this->s11E44->Size = System::Drawing::Size(64, 20);
			this->s11E44->TabIndex = 128;
			this->s11E44->Text = S"XXXX";
			// 
			// s11E244
			// 
			this->s11E244->Enabled = false;
			this->s11E244->Location = System::Drawing::Point(656, 32);
			this->s11E244->Name = S"s11E244";
			this->s11E244->Size = System::Drawing::Size(64, 20);
			this->s11E244->TabIndex = 127;
			this->s11E244->Text = S"XXXX";
			// 
			// s11E257
			// 
			this->s11E257->Enabled = false;
			this->s11E257->Location = System::Drawing::Point(656, 152);
			this->s11E257->Name = S"s11E257";
			this->s11E257->Size = System::Drawing::Size(64, 20);
			this->s11E257->TabIndex = 132;
			this->s11E257->Text = S"XXXX";
			// 
			// s11E95
			// 
			this->s11E95->Enabled = false;
			this->s11E95->Location = System::Drawing::Point(656, 128);
			this->s11E95->Name = S"s11E95";
			this->s11E95->Size = System::Drawing::Size(64, 20);
			this->s11E95->TabIndex = 131;
			this->s11E95->Text = S"XXXX";
			// 
			// s11E251
			// 
			this->s11E251->Enabled = false;
			this->s11E251->Location = System::Drawing::Point(656, 104);
			this->s11E251->Name = S"s11E251";
			this->s11E251->Size = System::Drawing::Size(64, 20);
			this->s11E251->TabIndex = 130;
			this->s11E251->Text = S"XXXX";
			// 
			// s11E254
			// 
			this->s11E254->Enabled = false;
			this->s11E254->Location = System::Drawing::Point(656, 224);
			this->s11E254->Name = S"s11E254";
			this->s11E254->Size = System::Drawing::Size(64, 20);
			this->s11E254->TabIndex = 135;
			this->s11E254->Text = S"XXXX";
			// 
			// s11E253
			// 
			this->s11E253->Enabled = false;
			this->s11E253->Location = System::Drawing::Point(656, 200);
			this->s11E253->Name = S"s11E253";
			this->s11E253->Size = System::Drawing::Size(64, 20);
			this->s11E253->TabIndex = 134;
			this->s11E253->Text = S"XXXX";
			// 
			// s11E258
			// 
			this->s11E258->Enabled = false;
			this->s11E258->Location = System::Drawing::Point(656, 176);
			this->s11E258->Name = S"s11E258";
			this->s11E258->Size = System::Drawing::Size(64, 20);
			this->s11E258->TabIndex = 133;
			this->s11E258->Text = S"XXXX";
			// 
			// s11E87
			// 
			this->s11E87->Enabled = false;
			this->s11E87->Location = System::Drawing::Point(656, 296);
			this->s11E87->Name = S"s11E87";
			this->s11E87->Size = System::Drawing::Size(64, 20);
			this->s11E87->TabIndex = 138;
			this->s11E87->Text = S"XXXX";
			// 
			// s11E256
			// 
			this->s11E256->Enabled = false;
			this->s11E256->Location = System::Drawing::Point(656, 272);
			this->s11E256->Name = S"s11E256";
			this->s11E256->Size = System::Drawing::Size(64, 20);
			this->s11E256->TabIndex = 137;
			this->s11E256->Text = S"XXXX";
			// 
			// s11E255
			// 
			this->s11E255->Enabled = false;
			this->s11E255->Location = System::Drawing::Point(656, 248);
			this->s11E255->Name = S"s11E255";
			this->s11E255->Size = System::Drawing::Size(64, 20);
			this->s11E255->TabIndex = 136;
			this->s11E255->Text = S"XXXX";
			// 
			// s11E332
			// 
			this->s11E332->Enabled = false;
			this->s11E332->Location = System::Drawing::Point(656, 368);
			this->s11E332->Name = S"s11E332";
			this->s11E332->Size = System::Drawing::Size(64, 20);
			this->s11E332->TabIndex = 141;
			this->s11E332->Text = S"XXXX";
			// 
			// s11E331
			// 
			this->s11E331->Enabled = false;
			this->s11E331->Location = System::Drawing::Point(656, 344);
			this->s11E331->Name = S"s11E331";
			this->s11E331->Size = System::Drawing::Size(64, 20);
			this->s11E331->TabIndex = 140;
			this->s11E331->Text = S"XXXX";
			// 
			// s11E88
			// 
			this->s11E88->Enabled = false;
			this->s11E88->Location = System::Drawing::Point(656, 320);
			this->s11E88->Name = S"s11E88";
			this->s11E88->Size = System::Drawing::Size(64, 20);
			this->s11E88->TabIndex = 139;
			this->s11E88->Text = S"XXXX";
			// 
			// s11E334
			// 
			this->s11E334->Enabled = false;
			this->s11E334->Location = System::Drawing::Point(656, 416);
			this->s11E334->Name = S"s11E334";
			this->s11E334->Size = System::Drawing::Size(64, 20);
			this->s11E334->TabIndex = 144;
			this->s11E334->Text = S"XXXX";
			// 
			// s11E333
			// 
			this->s11E333->Enabled = false;
			this->s11E333->Location = System::Drawing::Point(656, 392);
			this->s11E333->Name = S"s11E333";
			this->s11E333->Size = System::Drawing::Size(64, 20);
			this->s11E333->TabIndex = 143;
			this->s11E333->Text = S"XXXX";
			// 
			// s11E335
			// 
			this->s11E335->Enabled = false;
			this->s11E335->Location = System::Drawing::Point(656, 440);
			this->s11E335->Name = S"s11E335";
			this->s11E335->Size = System::Drawing::Size(64, 20);
			this->s11E335->TabIndex = 142;
			this->s11E335->Text = S"XXXX";
			// 
			// s11E336
			// 
			this->s11E336->Enabled = false;
			this->s11E336->Location = System::Drawing::Point(656, 464);
			this->s11E336->Name = S"s11E336";
			this->s11E336->Size = System::Drawing::Size(64, 20);
			this->s11E336->TabIndex = 145;
			this->s11E336->Text = S"XXXX";
			// 
			// s22A4
			// 
			this->s22A4->Enabled = false;
			this->s22A4->Location = System::Drawing::Point(288, 488);
			this->s22A4->Name = S"s22A4";
			this->s22A4->Size = System::Drawing::Size(64, 20);
			this->s22A4->TabIndex = 146;
			this->s22A4->Text = S"X.XXX A";
			// 
			// s12A16
			// 
			this->s12A16->Enabled = false;
			this->s12A16->Location = System::Drawing::Point(560, 488);
			this->s12A16->Name = S"s12A16";
			this->s12A16->Size = System::Drawing::Size(64, 20);
			this->s12A16->TabIndex = 147;
			this->s12A16->Text = S"X.XXX A";
			// 
			// s11E224
			// 
			this->s11E224->Enabled = false;
			this->s11E224->Location = System::Drawing::Point(384, 104);
			this->s11E224->Name = S"s11E224";
			this->s11E224->Size = System::Drawing::Size(64, 20);
			this->s11E224->TabIndex = 148;
			this->s11E224->Text = S"XXXX";
			// 
			// s22E168
			// 
			this->s22E168->Enabled = false;
			this->s22E168->Location = System::Drawing::Point(384, 80);
			this->s22E168->Name = S"s22E168";
			this->s22E168->Size = System::Drawing::Size(64, 20);
			this->s22E168->TabIndex = 156;
			this->s22E168->Text = S"XXXX";
			// 
			// s22E124
			// 
			this->s22E124->Enabled = false;
			this->s22E124->Location = System::Drawing::Point(384, 56);
			this->s22E124->Name = S"s22E124";
			this->s22E124->Size = System::Drawing::Size(64, 20);
			this->s22E124->TabIndex = 155;
			this->s22E124->Text = S"XXXX";
			// 
			// s11E226
			// 
			this->s11E226->Enabled = false;
			this->s11E226->Location = System::Drawing::Point(384, 128);
			this->s11E226->Name = S"s11E226";
			this->s11E226->Size = System::Drawing::Size(64, 20);
			this->s11E226->TabIndex = 153;
			this->s11E226->Text = S"XXXX";
			// 
			// label59
			// 
			this->label59->Location = System::Drawing::Point(224, 80);
			this->label59->Name = S"label59";
			this->label59->Size = System::Drawing::Size(144, 24);
			this->label59->TabIndex = 152;
			this->label59->Text = S"-ROLL/-Y ENG";
			this->label59->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label60
			// 
			this->label60->Location = System::Drawing::Point(224, 56);
			this->label60->Name = S"label60";
			this->label60->Size = System::Drawing::Size(144, 24);
			this->label60->TabIndex = 151;
			this->label60->Text = S"+ROLL/-Y ENG";
			this->label60->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label61
			// 
			this->label61->Location = System::Drawing::Point(224, 32);
			this->label61->Name = S"label61";
			this->label61->Size = System::Drawing::Size(144, 24);
			this->label61->TabIndex = 150;
			this->label61->Text = S"-ROLL/+Y ENG";
			this->label61->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label62
			// 
			this->label62->Location = System::Drawing::Point(224, 8);
			this->label62->Name = S"label62";
			this->label62->Size = System::Drawing::Size(144, 24);
			this->label62->TabIndex = 149;
			this->label62->Text = S"+ROLL/+Y ENG";
			this->label62->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// SCSForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(726, 516);
			this->Controls->Add(this->s22E168);
			this->Controls->Add(this->s22E124);
			this->Controls->Add(this->s11E226);
			this->Controls->Add(this->label59);
			this->Controls->Add(this->label60);
			this->Controls->Add(this->label61);
			this->Controls->Add(this->label62);
			this->Controls->Add(this->s11E224);
			this->Controls->Add(this->s12A16);
			this->Controls->Add(this->s22A4);
			this->Controls->Add(this->s11E336);
			this->Controls->Add(this->s11E334);
			this->Controls->Add(this->s11E333);
			this->Controls->Add(this->s11E335);
			this->Controls->Add(this->s11E332);
			this->Controls->Add(this->s11E331);
			this->Controls->Add(this->s11E88);
			this->Controls->Add(this->s11E87);
			this->Controls->Add(this->s11E256);
			this->Controls->Add(this->s11E255);
			this->Controls->Add(this->s11E254);
			this->Controls->Add(this->s11E253);
			this->Controls->Add(this->s11E258);
			this->Controls->Add(this->s11E257);
			this->Controls->Add(this->s11E95);
			this->Controls->Add(this->s11E251);
			this->Controls->Add(this->s11E94);
			this->Controls->Add(this->s11E44);
			this->Controls->Add(this->s11E244);
			this->Controls->Add(this->s11E243);
			this->Controls->Add(this->s11E93);
			this->Controls->Add(this->s11E86);
			this->Controls->Add(this->s11E85);
			this->Controls->Add(this->s11E84);
			this->Controls->Add(this->s11E82);
			this->Controls->Add(this->s12A13);
			this->Controls->Add(this->s12A15);
			this->Controls->Add(this->s11E242);
			this->Controls->Add(this->s11E238);
			this->Controls->Add(this->s11E236);
			this->Controls->Add(this->s11E228);
			this->Controls->Add(this->s22E113);
			this->Controls->Add(this->s22E157);
			this->Controls->Add(this->s22E102);
			this->Controls->Add(this->s22E146);
			this->Controls->Add(this->s22E135);
			this->Controls->Add(this->s22E91);
			this->Controls->Add(this->s22E88);
			this->Controls->Add(this->s22E77);
			this->Controls->Add(this->s22E66);
			this->Controls->Add(this->s22E55);
			this->Controls->Add(this->s22E44);
			this->Controls->Add(this->s22E33);
			this->Controls->Add(this->s22E22);
			this->Controls->Add(this->s22E11);
			this->Controls->Add(this->s51A13);
			this->Controls->Add(this->s51A12);
			this->Controls->Add(this->s51A11);
			this->Controls->Add(this->s12A10);
			this->Controls->Add(this->s12A8);
			this->Controls->Add(this->s12A7);
			this->Controls->Add(this->s12A6);
			this->Controls->Add(this->s12A5);
			this->Controls->Add(this->s12A4);
			this->Controls->Add(this->s51A6);
			this->Controls->Add(this->s51A5);
			this->Controls->Add(this->label58);
			this->Controls->Add(this->label57);
			this->Controls->Add(this->label55);
			this->Controls->Add(this->label56);
			this->Controls->Add(this->label53);
			this->Controls->Add(this->label54);
			this->Controls->Add(this->label52);
			this->Controls->Add(this->label51);
			this->Controls->Add(this->label50);
			this->Controls->Add(this->label49);
			this->Controls->Add(this->label47);
			this->Controls->Add(this->label48);
			this->Controls->Add(this->label45);
			this->Controls->Add(this->label46);
			this->Controls->Add(this->label44);
			this->Controls->Add(this->label43);
			this->Controls->Add(this->label42);
			this->Controls->Add(this->label41);
			this->Controls->Add(this->label40);
			this->Controls->Add(this->label39);
			this->Controls->Add(this->label38);
			this->Controls->Add(this->label37);
			this->Controls->Add(this->label36);
			this->Controls->Add(this->label35);
			this->Controls->Add(this->label34);
			this->Controls->Add(this->label33);
			this->Controls->Add(this->label32);
			this->Controls->Add(this->label31);
			this->Controls->Add(this->label29);
			this->Controls->Add(this->label30);
			this->Controls->Add(this->label28);
			this->Controls->Add(this->label27);
			this->Controls->Add(this->label25);
			this->Controls->Add(this->label26);
			this->Controls->Add(this->label23);
			this->Controls->Add(this->label24);
			this->Controls->Add(this->label22);
			this->Controls->Add(this->label21);
			this->Controls->Add(this->label17);
			this->Controls->Add(this->label18);
			this->Controls->Add(this->label19);
			this->Controls->Add(this->label20);
			this->Controls->Add(this->label13);
			this->Controls->Add(this->label14);
			this->Controls->Add(this->label15);
			this->Controls->Add(this->label16);
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
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = S"SCSForm";
			this->Text = S"STABILIZATION AND CONTROL SYSTEM";
			this->ResumeLayout(false);

		}		

};
}