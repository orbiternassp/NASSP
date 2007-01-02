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
	/// Summary for SPSForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	__gc class SPSForm : public System::Windows::Forms::Form
	{
	public: 
		SPSForm(void)
		{
			InitializeComponent();
			this->add_Closing(new CancelEventHandler(this,SPSForm_Closing));
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
	private: System::Void SPSForm_Closing(Object * sender, System::ComponentModel::CancelEventArgs* e){			
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
	private: System::Windows::Forms::TextBox *  s11A10;
	private: System::Windows::Forms::TextBox *  s11A11;
	private: System::Windows::Forms::TextBox *  s11A12;
	private: System::Windows::Forms::TextBox *  s10A10;
	private: System::Windows::Forms::TextBox *  s10A11;
	private: System::Windows::Forms::TextBox *  s12A14;
	private: System::Windows::Forms::TextBox *  s11A152;
	private: System::Windows::Forms::TextBox *  s11A161;
	private: System::Windows::Forms::TextBox *  s10A16;
	private: System::Windows::Forms::TextBox *  s10A49;
	private: System::Windows::Forms::TextBox *  s10A47;
	private: System::Windows::Forms::TextBox *  s10A59;
	private: System::Windows::Forms::TextBox *  s10A120;
	private: System::Windows::Forms::TextBox *  s10A44;
	private: System::Windows::Forms::TextBox *  s10A37;
	private: System::Windows::Forms::TextBox *  s10A117;
	private: System::Windows::Forms::TextBox *  s11A31;
	private: System::Windows::Forms::TextBox *  s11A32;
	private: System::Windows::Forms::TextBox *  s11A33;
	private: System::Windows::Forms::TextBox *  s11A34;
	private: System::Windows::Forms::TextBox *  s10A28;
	private: System::Windows::Forms::TextBox *  s10A26;
	private: System::Windows::Forms::TextBox *  s10A31;
	private: System::Windows::Forms::TextBox *  s10A29;
	private: System::Windows::Forms::GroupBox *  groupBox1;
	private: System::Windows::Forms::GroupBox *  groupBox2;
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
	private: System::Windows::Forms::TextBox *  s10A18;
	private: System::Windows::Forms::TextBox *  s10A21;
	private: System::Windows::Forms::TextBox *  s11A22;
	private: System::Windows::Forms::TextBox *  s11A21;
	private: System::Windows::Forms::TextBox *  s10A27;
	private: System::Windows::Forms::TextBox *  s10A24;
	private: System::Windows::Forms::TextBox *  s10A33;
	private: System::Windows::Forms::TextBox *  s10A30;





















	private: System::Windows::Forms::TextBox *  s11A165;
	private: System::Windows::Forms::TextBox *  s11A164;
	private: System::Windows::Forms::TextBox *  s11A163;
	private: System::Windows::Forms::TextBox *  s11A156;
	private: System::Windows::Forms::TextBox *  s11A155;
private: System::Windows::Forms::TextBox *  s11A166;
private: System::Windows::Forms::TextBox *  s10A20;
private: System::Windows::Forms::TextBox *  s10A19;
private: System::Windows::Forms::TextBox *  s10A17;
private: System::Windows::Forms::TextBox *  s10A136;
private: System::Windows::Forms::TextBox *  s10A105;
private: System::Windows::Forms::TextBox *  s10A102;
private: System::Windows::Forms::TextBox *  s10A99;
private: System::Windows::Forms::TextBox *  s10A93;
private: System::Windows::Forms::TextBox *  s11A130;
private: System::Windows::Forms::TextBox *  s11A112;
private: System::Windows::Forms::TextBox *  s11A111;
private: System::Windows::Forms::TextBox *  s11A28;
private: System::Windows::Forms::TextBox *  s11A27;
private: System::Windows::Forms::TextBox *  s11A46;
private: System::Windows::Forms::TextBox *  s11A94;
private: System::Windows::Forms::TextBox *  s11A24;
private: System::Windows::Forms::TextBox *  s11A40;
private: System::Windows::Forms::TextBox *  s11A92;
private: System::Windows::Forms::TextBox *  s11A23;
private: System::Windows::Forms::TextBox *  s11A39;





















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
			this->s11A10 = new System::Windows::Forms::TextBox();
			this->s11A11 = new System::Windows::Forms::TextBox();
			this->s11A12 = new System::Windows::Forms::TextBox();
			this->s10A10 = new System::Windows::Forms::TextBox();
			this->s10A11 = new System::Windows::Forms::TextBox();
			this->s12A14 = new System::Windows::Forms::TextBox();
			this->s11A152 = new System::Windows::Forms::TextBox();
			this->s11A161 = new System::Windows::Forms::TextBox();
			this->s10A16 = new System::Windows::Forms::TextBox();
			this->s10A49 = new System::Windows::Forms::TextBox();
			this->s10A47 = new System::Windows::Forms::TextBox();
			this->s10A59 = new System::Windows::Forms::TextBox();
			this->s10A120 = new System::Windows::Forms::TextBox();
			this->s10A44 = new System::Windows::Forms::TextBox();
			this->s10A37 = new System::Windows::Forms::TextBox();
			this->s10A117 = new System::Windows::Forms::TextBox();
			this->s11A31 = new System::Windows::Forms::TextBox();
			this->s11A32 = new System::Windows::Forms::TextBox();
			this->s11A33 = new System::Windows::Forms::TextBox();
			this->s11A34 = new System::Windows::Forms::TextBox();
			this->s10A28 = new System::Windows::Forms::TextBox();
			this->s10A26 = new System::Windows::Forms::TextBox();
			this->s10A31 = new System::Windows::Forms::TextBox();
			this->s10A29 = new System::Windows::Forms::TextBox();
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->groupBox2 = new System::Windows::Forms::GroupBox();
			this->s11A156 = new System::Windows::Forms::TextBox();
			this->s11A155 = new System::Windows::Forms::TextBox();
			this->s11A166 = new System::Windows::Forms::TextBox();
			this->s11A165 = new System::Windows::Forms::TextBox();
			this->s11A164 = new System::Windows::Forms::TextBox();
			this->s11A163 = new System::Windows::Forms::TextBox();
			this->s10A20 = new System::Windows::Forms::TextBox();
			this->s10A19 = new System::Windows::Forms::TextBox();
			this->s10A17 = new System::Windows::Forms::TextBox();
			this->s10A136 = new System::Windows::Forms::TextBox();
			this->s10A105 = new System::Windows::Forms::TextBox();
			this->s10A102 = new System::Windows::Forms::TextBox();
			this->s10A99 = new System::Windows::Forms::TextBox();
			this->s10A93 = new System::Windows::Forms::TextBox();
			this->s11A130 = new System::Windows::Forms::TextBox();
			this->s11A112 = new System::Windows::Forms::TextBox();
			this->s11A111 = new System::Windows::Forms::TextBox();
			this->s11A28 = new System::Windows::Forms::TextBox();
			this->s11A27 = new System::Windows::Forms::TextBox();
			this->s11A46 = new System::Windows::Forms::TextBox();
			this->s11A94 = new System::Windows::Forms::TextBox();
			this->s11A24 = new System::Windows::Forms::TextBox();
			this->s11A40 = new System::Windows::Forms::TextBox();
			this->s11A92 = new System::Windows::Forms::TextBox();
			this->s11A23 = new System::Windows::Forms::TextBox();
			this->s11A39 = new System::Windows::Forms::TextBox();
			this->s10A33 = new System::Windows::Forms::TextBox();
			this->s10A30 = new System::Windows::Forms::TextBox();
			this->s10A27 = new System::Windows::Forms::TextBox();
			this->s10A24 = new System::Windows::Forms::TextBox();
			this->s11A22 = new System::Windows::Forms::TextBox();
			this->s11A21 = new System::Windows::Forms::TextBox();
			this->s10A21 = new System::Windows::Forms::TextBox();
			this->s10A18 = new System::Windows::Forms::TextBox();
			this->label56 = new System::Windows::Forms::Label();
			this->label57 = new System::Windows::Forms::Label();
			this->label58 = new System::Windows::Forms::Label();
			this->label53 = new System::Windows::Forms::Label();
			this->label54 = new System::Windows::Forms::Label();
			this->label55 = new System::Windows::Forms::Label();
			this->label50 = new System::Windows::Forms::Label();
			this->label51 = new System::Windows::Forms::Label();
			this->label52 = new System::Windows::Forms::Label();
			this->label49 = new System::Windows::Forms::Label();
			this->label48 = new System::Windows::Forms::Label();
			this->label47 = new System::Windows::Forms::Label();
			this->label46 = new System::Windows::Forms::Label();
			this->label45 = new System::Windows::Forms::Label();
			this->label44 = new System::Windows::Forms::Label();
			this->label43 = new System::Windows::Forms::Label();
			this->label42 = new System::Windows::Forms::Label();
			this->label41 = new System::Windows::Forms::Label();
			this->label40 = new System::Windows::Forms::Label();
			this->label39 = new System::Windows::Forms::Label();
			this->label35 = new System::Windows::Forms::Label();
			this->label36 = new System::Windows::Forms::Label();
			this->label37 = new System::Windows::Forms::Label();
			this->label38 = new System::Windows::Forms::Label();
			this->label34 = new System::Windows::Forms::Label();
			this->label33 = new System::Windows::Forms::Label();
			this->label31 = new System::Windows::Forms::Label();
			this->label32 = new System::Windows::Forms::Label();
			this->label30 = new System::Windows::Forms::Label();
			this->label29 = new System::Windows::Forms::Label();
			this->label28 = new System::Windows::Forms::Label();
			this->label27 = new System::Windows::Forms::Label();
			this->label26 = new System::Windows::Forms::Label();
			this->label25 = new System::Windows::Forms::Label();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 16);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(152, 24);
			this->label1->TabIndex = 4;
			this->label1->Text = S"HE TK PRESS";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 40);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(152, 24);
			this->label2->TabIndex = 5;
			this->label2->Text = S"HE TK TEMP";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 64);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(152, 24);
			this->label3->TabIndex = 6;
			this->label3->Text = S"OX TK PRESS";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(8, 88);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(152, 24);
			this->label4->TabIndex = 7;
			this->label4->Text = S"FU TK PRESS";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(8, 112);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(152, 24);
			this->label5->TabIndex = 8;
			this->label5->Text = S"FU/OX VLV 1 POS";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(8, 136);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(152, 24);
			this->label6->TabIndex = 9;
			this->label6->Text = S"FU/OX VLV 2 POS";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(8, 160);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(152, 24);
			this->label7->TabIndex = 10;
			this->label7->Text = S"FU/OX VLV 3 POS";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(8, 184);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(152, 24);
			this->label8->TabIndex = 11;
			this->label8->Text = S"FU/OX VLV 4 POS";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(8, 208);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(152, 24);
			this->label9->TabIndex = 12;
			this->label9->Text = S"SPS VLV BODY TEMP";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 232);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(152, 24);
			this->label10->TabIndex = 13;
			this->label10->Text = S"SPS FU FEED LN TEMP";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(8, 256);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(152, 24);
			this->label11->TabIndex = 14;
			this->label11->Text = S"SPS OX FEED LN TEMP";
			this->label11->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label12
			// 
			this->label12->Location = System::Drawing::Point(8, 280);
			this->label12->Name = S"label12";
			this->label12->Size = System::Drawing::Size(152, 24);
			this->label12->TabIndex = 15;
			this->label12->Text = S"OX LINE 1 TEMP";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label13
			// 
			this->label13->Location = System::Drawing::Point(8, 304);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(152, 24);
			this->label13->TabIndex = 16;
			this->label13->Text = S"FU LINE 1 TEMP";
			this->label13->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label14
			// 
			this->label14->Location = System::Drawing::Point(8, 328);
			this->label14->Name = S"label14";
			this->label14->Size = System::Drawing::Size(152, 24);
			this->label14->TabIndex = 17;
			this->label14->Text = S"SPS INJ FLANGE TEMP 1";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label15
			// 
			this->label15->Location = System::Drawing::Point(8, 352);
			this->label15->Name = S"label15";
			this->label15->Size = System::Drawing::Size(152, 24);
			this->label15->TabIndex = 18;
			this->label15->Text = S"SPS INJ FLANGE TEMP 2";
			this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label16
			// 
			this->label16->Location = System::Drawing::Point(8, 376);
			this->label16->Name = S"label16";
			this->label16->Size = System::Drawing::Size(152, 24);
			this->label16->TabIndex = 19;
			this->label16->Text = S"SPS VLV ACT PRESS PRI";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label17
			// 
			this->label17->Location = System::Drawing::Point(8, 400);
			this->label17->Name = S"label17";
			this->label17->Size = System::Drawing::Size(152, 24);
			this->label17->TabIndex = 20;
			this->label17->Text = S"SPS VLV ACT PRESS SEC";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label18
			// 
			this->label18->Location = System::Drawing::Point(8, 424);
			this->label18->Name = S"label18";
			this->label18->Size = System::Drawing::Size(152, 24);
			this->label18->TabIndex = 21;
			this->label18->Text = S"OX TK 1 QTY - TOTAL AUX";
			this->label18->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label19
			// 
			this->label19->Location = System::Drawing::Point(8, 448);
			this->label19->Name = S"label19";
			this->label19->Size = System::Drawing::Size(152, 24);
			this->label19->TabIndex = 22;
			this->label19->Text = S"OX TK 2 QTY";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label20
			// 
			this->label20->Location = System::Drawing::Point(8, 496);
			this->label20->Name = S"label20";
			this->label20->Size = System::Drawing::Size(152, 24);
			this->label20->TabIndex = 24;
			this->label20->Text = S"FU TK 2 QTY";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label21
			// 
			this->label21->Location = System::Drawing::Point(8, 472);
			this->label21->Name = S"label21";
			this->label21->Size = System::Drawing::Size(152, 24);
			this->label21->TabIndex = 23;
			this->label21->Text = S"FU TK 1 QTY - TOTAL AUX";
			this->label21->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label22
			// 
			this->label22->Location = System::Drawing::Point(8, 520);
			this->label22->Name = S"label22";
			this->label22->Size = System::Drawing::Size(152, 24);
			this->label22->TabIndex = 25;
			this->label22->Text = S"ENG CHAMBER PRESS";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label23
			// 
			this->label23->Location = System::Drawing::Point(8, 544);
			this->label23->Name = S"label23";
			this->label23->Size = System::Drawing::Size(152, 24);
			this->label23->TabIndex = 26;
			this->label23->Text = S"FU SM/ENG INTERFACE P";
			this->label23->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label24
			// 
			this->label24->Location = System::Drawing::Point(8, 568);
			this->label24->Name = S"label24";
			this->label24->Size = System::Drawing::Size(152, 24);
			this->label24->TabIndex = 27;
			this->label24->Text = S"OX SM/ENG INTERFACE P";
			this->label24->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s11A10
			// 
			this->s11A10->Enabled = false;
			this->s11A10->Location = System::Drawing::Point(160, 16);
			this->s11A10->Name = S"s11A10";
			this->s11A10->Size = System::Drawing::Size(64, 20);
			this->s11A10->TabIndex = 28;
			this->s11A10->Text = S"XXXX PSIA";
			// 
			// s11A11
			// 
			this->s11A11->Enabled = false;
			this->s11A11->Location = System::Drawing::Point(160, 64);
			this->s11A11->Name = S"s11A11";
			this->s11A11->Size = System::Drawing::Size(64, 20);
			this->s11A11->TabIndex = 29;
			this->s11A11->Text = S"XXXX PSIA";
			// 
			// s11A12
			// 
			this->s11A12->Enabled = false;
			this->s11A12->Location = System::Drawing::Point(160, 88);
			this->s11A12->Name = S"s11A12";
			this->s11A12->Size = System::Drawing::Size(64, 20);
			this->s11A12->TabIndex = 30;
			this->s11A12->Text = S"XXXX PSIA";
			// 
			// s10A10
			// 
			this->s10A10->Enabled = false;
			this->s10A10->Location = System::Drawing::Point(160, 376);
			this->s10A10->Name = S"s10A10";
			this->s10A10->Size = System::Drawing::Size(64, 20);
			this->s10A10->TabIndex = 31;
			this->s10A10->Text = S"XXXX PSIA";
			// 
			// s10A11
			// 
			this->s10A11->Enabled = false;
			this->s10A11->Location = System::Drawing::Point(160, 400);
			this->s10A11->Name = S"s10A11";
			this->s10A11->Size = System::Drawing::Size(64, 20);
			this->s10A11->TabIndex = 32;
			this->s10A11->Text = S"XXXX PSIA";
			// 
			// s12A14
			// 
			this->s12A14->Enabled = false;
			this->s12A14->Location = System::Drawing::Point(160, 520);
			this->s12A14->Name = S"s12A14";
			this->s12A14->Size = System::Drawing::Size(64, 20);
			this->s12A14->TabIndex = 33;
			this->s12A14->Text = S"XXXX PSIA";
			// 
			// s11A152
			// 
			this->s11A152->Enabled = false;
			this->s11A152->Location = System::Drawing::Point(160, 544);
			this->s11A152->Name = S"s11A152";
			this->s11A152->Size = System::Drawing::Size(64, 20);
			this->s11A152->TabIndex = 34;
			this->s11A152->Text = S"XXXX PSIA";
			// 
			// s11A161
			// 
			this->s11A161->Enabled = false;
			this->s11A161->Location = System::Drawing::Point(160, 568);
			this->s11A161->Name = S"s11A161";
			this->s11A161->Size = System::Drawing::Size(64, 20);
			this->s11A161->TabIndex = 35;
			this->s11A161->Text = S"XXXX PSIA";
			// 
			// s10A16
			// 
			this->s10A16->Enabled = false;
			this->s10A16->Location = System::Drawing::Point(160, 40);
			this->s10A16->Name = S"s10A16";
			this->s10A16->Size = System::Drawing::Size(64, 20);
			this->s10A16->TabIndex = 36;
			this->s10A16->Text = S"XXX.XX °F";
			// 
			// s10A49
			// 
			this->s10A49->Enabled = false;
			this->s10A49->Location = System::Drawing::Point(160, 352);
			this->s10A49->Name = S"s10A49";
			this->s10A49->Size = System::Drawing::Size(64, 20);
			this->s10A49->TabIndex = 37;
			this->s10A49->Text = S"XXX.XX °F";
			// 
			// s10A47
			// 
			this->s10A47->Enabled = false;
			this->s10A47->Location = System::Drawing::Point(160, 328);
			this->s10A47->Name = S"s10A47";
			this->s10A47->Size = System::Drawing::Size(64, 20);
			this->s10A47->TabIndex = 38;
			this->s10A47->Text = S"XXX.XX °F";
			// 
			// s10A59
			// 
			this->s10A59->Enabled = false;
			this->s10A59->Location = System::Drawing::Point(160, 304);
			this->s10A59->Name = S"s10A59";
			this->s10A59->Size = System::Drawing::Size(64, 20);
			this->s10A59->TabIndex = 39;
			this->s10A59->Text = S"XXX.XX °F";
			// 
			// s10A120
			// 
			this->s10A120->Enabled = false;
			this->s10A120->Location = System::Drawing::Point(160, 256);
			this->s10A120->Name = S"s10A120";
			this->s10A120->Size = System::Drawing::Size(64, 20);
			this->s10A120->TabIndex = 41;
			this->s10A120->Text = S"XXX.XX °F";
			// 
			// s10A44
			// 
			this->s10A44->Enabled = false;
			this->s10A44->Location = System::Drawing::Point(160, 280);
			this->s10A44->Name = S"s10A44";
			this->s10A44->Size = System::Drawing::Size(64, 20);
			this->s10A44->TabIndex = 40;
			this->s10A44->Text = S"XXX.XX °F";
			// 
			// s10A37
			// 
			this->s10A37->Enabled = false;
			this->s10A37->Location = System::Drawing::Point(160, 208);
			this->s10A37->Name = S"s10A37";
			this->s10A37->Size = System::Drawing::Size(64, 20);
			this->s10A37->TabIndex = 43;
			this->s10A37->Text = S"XXX.XX °F";
			// 
			// s10A117
			// 
			this->s10A117->Enabled = false;
			this->s10A117->Location = System::Drawing::Point(160, 232);
			this->s10A117->Name = S"s10A117";
			this->s10A117->Size = System::Drawing::Size(64, 20);
			this->s10A117->TabIndex = 42;
			this->s10A117->Text = S"XXX.XX °F";
			// 
			// s11A31
			// 
			this->s11A31->Enabled = false;
			this->s11A31->Location = System::Drawing::Point(160, 112);
			this->s11A31->Name = S"s11A31";
			this->s11A31->Size = System::Drawing::Size(64, 20);
			this->s11A31->TabIndex = 44;
			this->s11A31->Text = S"XXX.XX °";
			// 
			// s11A32
			// 
			this->s11A32->Enabled = false;
			this->s11A32->Location = System::Drawing::Point(160, 136);
			this->s11A32->Name = S"s11A32";
			this->s11A32->Size = System::Drawing::Size(64, 20);
			this->s11A32->TabIndex = 45;
			this->s11A32->Text = S"XXX.XX °";
			// 
			// s11A33
			// 
			this->s11A33->Enabled = false;
			this->s11A33->Location = System::Drawing::Point(160, 160);
			this->s11A33->Name = S"s11A33";
			this->s11A33->Size = System::Drawing::Size(64, 20);
			this->s11A33->TabIndex = 46;
			this->s11A33->Text = S"XXX.XX °";
			// 
			// s11A34
			// 
			this->s11A34->Enabled = false;
			this->s11A34->Location = System::Drawing::Point(160, 184);
			this->s11A34->Name = S"s11A34";
			this->s11A34->Size = System::Drawing::Size(64, 20);
			this->s11A34->TabIndex = 47;
			this->s11A34->Text = S"XXX.XX °";
			// 
			// s10A28
			// 
			this->s10A28->Enabled = false;
			this->s10A28->Location = System::Drawing::Point(160, 448);
			this->s10A28->Name = S"s10A28";
			this->s10A28->Size = System::Drawing::Size(64, 20);
			this->s10A28->TabIndex = 49;
			this->s10A28->Text = S"XXX.XX%";
			// 
			// s10A26
			// 
			this->s10A26->Enabled = false;
			this->s10A26->Location = System::Drawing::Point(160, 424);
			this->s10A26->Name = S"s10A26";
			this->s10A26->Size = System::Drawing::Size(64, 20);
			this->s10A26->TabIndex = 48;
			this->s10A26->Text = S"XXX.XX%";
			// 
			// s10A31
			// 
			this->s10A31->Enabled = false;
			this->s10A31->Location = System::Drawing::Point(160, 496);
			this->s10A31->Name = S"s10A31";
			this->s10A31->Size = System::Drawing::Size(64, 20);
			this->s10A31->TabIndex = 51;
			this->s10A31->Text = S"XXX.XX%";
			// 
			// s10A29
			// 
			this->s10A29->Enabled = false;
			this->s10A29->Location = System::Drawing::Point(160, 472);
			this->s10A29->Name = S"s10A29";
			this->s10A29->Size = System::Drawing::Size(64, 20);
			this->s10A29->TabIndex = 50;
			this->s10A29->Text = S"XXX.XX%";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label16);
			this->groupBox1->Controls->Add(this->s10A11);
			this->groupBox1->Controls->Add(this->s10A28);
			this->groupBox1->Controls->Add(this->s11A32);
			this->groupBox1->Controls->Add(this->s10A37);
			this->groupBox1->Controls->Add(this->s10A117);
			this->groupBox1->Controls->Add(this->label7);
			this->groupBox1->Controls->Add(this->s10A10);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->s11A10);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->s10A49);
			this->groupBox1->Controls->Add(this->label23);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label9);
			this->groupBox1->Controls->Add(this->s11A11);
			this->groupBox1->Controls->Add(this->s11A152);
			this->groupBox1->Controls->Add(this->s10A44);
			this->groupBox1->Controls->Add(this->s11A33);
			this->groupBox1->Controls->Add(this->label11);
			this->groupBox1->Controls->Add(this->s12A14);
			this->groupBox1->Controls->Add(this->s10A26);
			this->groupBox1->Controls->Add(this->s11A161);
			this->groupBox1->Controls->Add(this->label17);
			this->groupBox1->Controls->Add(this->s10A31);
			this->groupBox1->Controls->Add(this->s10A29);
			this->groupBox1->Controls->Add(this->label10);
			this->groupBox1->Controls->Add(this->s10A47);
			this->groupBox1->Controls->Add(this->label20);
			this->groupBox1->Controls->Add(this->label21);
			this->groupBox1->Controls->Add(this->s10A59);
			this->groupBox1->Controls->Add(this->s11A34);
			this->groupBox1->Controls->Add(this->label22);
			this->groupBox1->Controls->Add(this->s11A31);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->label18);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->s11A12);
			this->groupBox1->Controls->Add(this->label8);
			this->groupBox1->Controls->Add(this->s10A120);
			this->groupBox1->Controls->Add(this->label12);
			this->groupBox1->Controls->Add(this->label14);
			this->groupBox1->Controls->Add(this->s10A16);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->label13);
			this->groupBox1->Controls->Add(this->label24);
			this->groupBox1->Controls->Add(this->label19);
			this->groupBox1->Controls->Add(this->label15);
			this->groupBox1->Location = System::Drawing::Point(8, 8);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(232, 592);
			this->groupBox1->TabIndex = 52;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"SERVICE PROPULSION SYSTEM";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->s11A156);
			this->groupBox2->Controls->Add(this->s11A155);
			this->groupBox2->Controls->Add(this->s11A166);
			this->groupBox2->Controls->Add(this->s11A165);
			this->groupBox2->Controls->Add(this->s11A164);
			this->groupBox2->Controls->Add(this->s11A163);
			this->groupBox2->Controls->Add(this->s10A20);
			this->groupBox2->Controls->Add(this->s10A19);
			this->groupBox2->Controls->Add(this->s10A17);
			this->groupBox2->Controls->Add(this->s10A136);
			this->groupBox2->Controls->Add(this->s10A105);
			this->groupBox2->Controls->Add(this->s10A102);
			this->groupBox2->Controls->Add(this->s10A99);
			this->groupBox2->Controls->Add(this->s10A93);
			this->groupBox2->Controls->Add(this->s11A130);
			this->groupBox2->Controls->Add(this->s11A112);
			this->groupBox2->Controls->Add(this->s11A111);
			this->groupBox2->Controls->Add(this->s11A28);
			this->groupBox2->Controls->Add(this->s11A27);
			this->groupBox2->Controls->Add(this->s11A46);
			this->groupBox2->Controls->Add(this->s11A94);
			this->groupBox2->Controls->Add(this->s11A24);
			this->groupBox2->Controls->Add(this->s11A40);
			this->groupBox2->Controls->Add(this->s11A92);
			this->groupBox2->Controls->Add(this->s11A23);
			this->groupBox2->Controls->Add(this->s11A39);
			this->groupBox2->Controls->Add(this->s10A33);
			this->groupBox2->Controls->Add(this->s10A30);
			this->groupBox2->Controls->Add(this->s10A27);
			this->groupBox2->Controls->Add(this->s10A24);
			this->groupBox2->Controls->Add(this->s11A22);
			this->groupBox2->Controls->Add(this->s11A21);
			this->groupBox2->Controls->Add(this->s10A21);
			this->groupBox2->Controls->Add(this->s10A18);
			this->groupBox2->Controls->Add(this->label56);
			this->groupBox2->Controls->Add(this->label57);
			this->groupBox2->Controls->Add(this->label58);
			this->groupBox2->Controls->Add(this->label53);
			this->groupBox2->Controls->Add(this->label54);
			this->groupBox2->Controls->Add(this->label55);
			this->groupBox2->Controls->Add(this->label50);
			this->groupBox2->Controls->Add(this->label51);
			this->groupBox2->Controls->Add(this->label52);
			this->groupBox2->Controls->Add(this->label49);
			this->groupBox2->Controls->Add(this->label48);
			this->groupBox2->Controls->Add(this->label47);
			this->groupBox2->Controls->Add(this->label46);
			this->groupBox2->Controls->Add(this->label45);
			this->groupBox2->Controls->Add(this->label44);
			this->groupBox2->Controls->Add(this->label43);
			this->groupBox2->Controls->Add(this->label42);
			this->groupBox2->Controls->Add(this->label41);
			this->groupBox2->Controls->Add(this->label40);
			this->groupBox2->Controls->Add(this->label39);
			this->groupBox2->Controls->Add(this->label35);
			this->groupBox2->Controls->Add(this->label36);
			this->groupBox2->Controls->Add(this->label37);
			this->groupBox2->Controls->Add(this->label38);
			this->groupBox2->Controls->Add(this->label34);
			this->groupBox2->Controls->Add(this->label33);
			this->groupBox2->Controls->Add(this->label31);
			this->groupBox2->Controls->Add(this->label32);
			this->groupBox2->Controls->Add(this->label30);
			this->groupBox2->Controls->Add(this->label29);
			this->groupBox2->Controls->Add(this->label28);
			this->groupBox2->Controls->Add(this->label27);
			this->groupBox2->Controls->Add(this->label26);
			this->groupBox2->Controls->Add(this->label25);
			this->groupBox2->Location = System::Drawing::Point(248, 8);
			this->groupBox2->Name = S"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(448, 592);
			this->groupBox2->TabIndex = 53;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = S"REACTION CONTROL SYSTEM";
			// 
			// s11A156
			// 
			this->s11A156->Enabled = false;
			this->s11A156->Location = System::Drawing::Point(176, 88);
			this->s11A156->Name = S"s11A156";
			this->s11A156->Size = System::Drawing::Size(64, 20);
			this->s11A156->TabIndex = 72;
			this->s11A156->Text = S"XXX.XX °F";
			// 
			// s11A155
			// 
			this->s11A155->Enabled = false;
			this->s11A155->Location = System::Drawing::Point(176, 64);
			this->s11A155->Name = S"s11A155";
			this->s11A155->Size = System::Drawing::Size(64, 20);
			this->s11A155->TabIndex = 71;
			this->s11A155->Text = S"XXX.XX °F";
			// 
			// s11A166
			// 
			this->s11A166->Enabled = false;
			this->s11A166->Location = System::Drawing::Point(176, 328);
			this->s11A166->Name = S"s11A166";
			this->s11A166->Size = System::Drawing::Size(64, 20);
			this->s11A166->TabIndex = 70;
			this->s11A166->Text = S"XXX.XX °F";
			// 
			// s11A165
			// 
			this->s11A165->Enabled = false;
			this->s11A165->Location = System::Drawing::Point(176, 304);
			this->s11A165->Name = S"s11A165";
			this->s11A165->Size = System::Drawing::Size(64, 20);
			this->s11A165->TabIndex = 69;
			this->s11A165->Text = S"XXX.XX °F";
			// 
			// s11A164
			// 
			this->s11A164->Enabled = false;
			this->s11A164->Location = System::Drawing::Point(176, 280);
			this->s11A164->Name = S"s11A164";
			this->s11A164->Size = System::Drawing::Size(64, 20);
			this->s11A164->TabIndex = 68;
			this->s11A164->Text = S"XXX.XX °F";
			// 
			// s11A163
			// 
			this->s11A163->Enabled = false;
			this->s11A163->Location = System::Drawing::Point(176, 256);
			this->s11A163->Name = S"s11A163";
			this->s11A163->Size = System::Drawing::Size(64, 20);
			this->s11A163->TabIndex = 67;
			this->s11A163->Text = S"XXX.XX °F";
			// 
			// s10A20
			// 
			this->s10A20->Enabled = false;
			this->s10A20->Location = System::Drawing::Point(176, 520);
			this->s10A20->Name = S"s10A20";
			this->s10A20->Size = System::Drawing::Size(64, 20);
			this->s10A20->TabIndex = 66;
			this->s10A20->Text = S"XXX.XX °F";
			// 
			// s10A19
			// 
			this->s10A19->Enabled = false;
			this->s10A19->Location = System::Drawing::Point(176, 496);
			this->s10A19->Name = S"s10A19";
			this->s10A19->Size = System::Drawing::Size(64, 20);
			this->s10A19->TabIndex = 65;
			this->s10A19->Text = S"XXX.XX °F";
			// 
			// s10A17
			// 
			this->s10A17->Enabled = false;
			this->s10A17->Location = System::Drawing::Point(176, 472);
			this->s10A17->Name = S"s10A17";
			this->s10A17->Size = System::Drawing::Size(64, 20);
			this->s10A17->TabIndex = 64;
			this->s10A17->Text = S"XXX.XX °F";
			// 
			// s10A136
			// 
			this->s10A136->Enabled = false;
			this->s10A136->Location = System::Drawing::Point(176, 448);
			this->s10A136->Name = S"s10A136";
			this->s10A136->Size = System::Drawing::Size(64, 20);
			this->s10A136->TabIndex = 63;
			this->s10A136->Text = S"XXX.XX °F";
			// 
			// s10A105
			// 
			this->s10A105->Enabled = false;
			this->s10A105->Location = System::Drawing::Point(176, 424);
			this->s10A105->Name = S"s10A105";
			this->s10A105->Size = System::Drawing::Size(64, 20);
			this->s10A105->TabIndex = 62;
			this->s10A105->Text = S"XXX.XX%";
			// 
			// s10A102
			// 
			this->s10A102->Enabled = false;
			this->s10A102->Location = System::Drawing::Point(176, 400);
			this->s10A102->Name = S"s10A102";
			this->s10A102->Size = System::Drawing::Size(64, 20);
			this->s10A102->TabIndex = 61;
			this->s10A102->Text = S"XXX.XX%";
			// 
			// s10A99
			// 
			this->s10A99->Enabled = false;
			this->s10A99->Location = System::Drawing::Point(176, 376);
			this->s10A99->Name = S"s10A99";
			this->s10A99->Size = System::Drawing::Size(64, 20);
			this->s10A99->TabIndex = 60;
			this->s10A99->Text = S"XXX.XX%";
			// 
			// s10A93
			// 
			this->s10A93->Enabled = false;
			this->s10A93->Location = System::Drawing::Point(176, 352);
			this->s10A93->Name = S"s10A93";
			this->s10A93->Size = System::Drawing::Size(64, 20);
			this->s10A93->TabIndex = 59;
			this->s10A93->Text = S"XXX.XX%";
			// 
			// s11A130
			// 
			this->s11A130->Enabled = false;
			this->s11A130->Location = System::Drawing::Point(376, 280);
			this->s11A130->Name = S"s11A130";
			this->s11A130->Size = System::Drawing::Size(64, 20);
			this->s11A130->TabIndex = 58;
			this->s11A130->Text = S"XXXX PSIA";
			// 
			// s11A112
			// 
			this->s11A112->Enabled = false;
			this->s11A112->Location = System::Drawing::Point(376, 256);
			this->s11A112->Name = S"s11A112";
			this->s11A112->Size = System::Drawing::Size(64, 20);
			this->s11A112->TabIndex = 57;
			this->s11A112->Text = S"XXXX PSIA";
			// 
			// s11A111
			// 
			this->s11A111->Enabled = false;
			this->s11A111->Location = System::Drawing::Point(376, 232);
			this->s11A111->Name = S"s11A111";
			this->s11A111->Size = System::Drawing::Size(64, 20);
			this->s11A111->TabIndex = 56;
			this->s11A111->Text = S"XXXX PSIA";
			// 
			// s11A28
			// 
			this->s11A28->Enabled = false;
			this->s11A28->Location = System::Drawing::Point(376, 208);
			this->s11A28->Name = S"s11A28";
			this->s11A28->Size = System::Drawing::Size(64, 20);
			this->s11A28->TabIndex = 55;
			this->s11A28->Text = S"XXXX PSIA";
			// 
			// s11A27
			// 
			this->s11A27->Enabled = false;
			this->s11A27->Location = System::Drawing::Point(376, 184);
			this->s11A27->Name = S"s11A27";
			this->s11A27->Size = System::Drawing::Size(64, 20);
			this->s11A27->TabIndex = 54;
			this->s11A27->Text = S"XXXX PSIA";
			// 
			// s11A46
			// 
			this->s11A46->Enabled = false;
			this->s11A46->Location = System::Drawing::Point(376, 160);
			this->s11A46->Name = S"s11A46";
			this->s11A46->Size = System::Drawing::Size(64, 20);
			this->s11A46->TabIndex = 53;
			this->s11A46->Text = S"XXXX PSIA";
			// 
			// s11A94
			// 
			this->s11A94->Enabled = false;
			this->s11A94->Location = System::Drawing::Point(376, 136);
			this->s11A94->Name = S"s11A94";
			this->s11A94->Size = System::Drawing::Size(64, 20);
			this->s11A94->TabIndex = 52;
			this->s11A94->Text = S"XXXX PSIA";
			// 
			// s11A24
			// 
			this->s11A24->Enabled = false;
			this->s11A24->Location = System::Drawing::Point(376, 112);
			this->s11A24->Name = S"s11A24";
			this->s11A24->Size = System::Drawing::Size(64, 20);
			this->s11A24->TabIndex = 51;
			this->s11A24->Text = S"XXXX PSIA";
			// 
			// s11A40
			// 
			this->s11A40->Enabled = false;
			this->s11A40->Location = System::Drawing::Point(376, 88);
			this->s11A40->Name = S"s11A40";
			this->s11A40->Size = System::Drawing::Size(64, 20);
			this->s11A40->TabIndex = 50;
			this->s11A40->Text = S"XXXX PSIA";
			// 
			// s11A92
			// 
			this->s11A92->Enabled = false;
			this->s11A92->Location = System::Drawing::Point(376, 64);
			this->s11A92->Name = S"s11A92";
			this->s11A92->Size = System::Drawing::Size(64, 20);
			this->s11A92->TabIndex = 49;
			this->s11A92->Text = S"XXXX PSIA";
			// 
			// s11A23
			// 
			this->s11A23->Enabled = false;
			this->s11A23->Location = System::Drawing::Point(376, 40);
			this->s11A23->Name = S"s11A23";
			this->s11A23->Size = System::Drawing::Size(64, 20);
			this->s11A23->TabIndex = 48;
			this->s11A23->Text = S"XXXX PSIA";
			// 
			// s11A39
			// 
			this->s11A39->Enabled = false;
			this->s11A39->Location = System::Drawing::Point(376, 16);
			this->s11A39->Name = S"s11A39";
			this->s11A39->Size = System::Drawing::Size(64, 20);
			this->s11A39->TabIndex = 47;
			this->s11A39->Text = S"XXXX PSIA";
			// 
			// s10A33
			// 
			this->s10A33->Enabled = false;
			this->s10A33->Location = System::Drawing::Point(176, 232);
			this->s10A33->Name = S"s10A33";
			this->s10A33->Size = System::Drawing::Size(64, 20);
			this->s10A33->TabIndex = 46;
			this->s10A33->Text = S"XXXX PSIA";
			// 
			// s10A30
			// 
			this->s10A30->Enabled = false;
			this->s10A30->Location = System::Drawing::Point(176, 208);
			this->s10A30->Name = S"s10A30";
			this->s10A30->Size = System::Drawing::Size(64, 20);
			this->s10A30->TabIndex = 45;
			this->s10A30->Text = S"XXXX PSIA";
			// 
			// s10A27
			// 
			this->s10A27->Enabled = false;
			this->s10A27->Location = System::Drawing::Point(176, 184);
			this->s10A27->Name = S"s10A27";
			this->s10A27->Size = System::Drawing::Size(64, 20);
			this->s10A27->TabIndex = 44;
			this->s10A27->Text = S"XXXX PSIA";
			// 
			// s10A24
			// 
			this->s10A24->Enabled = false;
			this->s10A24->Location = System::Drawing::Point(176, 160);
			this->s10A24->Name = S"s10A24";
			this->s10A24->Size = System::Drawing::Size(64, 20);
			this->s10A24->TabIndex = 43;
			this->s10A24->Text = S"XXXX PSIA";
			// 
			// s11A22
			// 
			this->s11A22->Enabled = false;
			this->s11A22->Location = System::Drawing::Point(176, 136);
			this->s11A22->Name = S"s11A22";
			this->s11A22->Size = System::Drawing::Size(64, 20);
			this->s11A22->TabIndex = 42;
			this->s11A22->Text = S"XXXX PSIA";
			// 
			// s11A21
			// 
			this->s11A21->Enabled = false;
			this->s11A21->Location = System::Drawing::Point(176, 112);
			this->s11A21->Name = S"s11A21";
			this->s11A21->Size = System::Drawing::Size(64, 20);
			this->s11A21->TabIndex = 41;
			this->s11A21->Text = S"XXXX PSIA";
			// 
			// s10A21
			// 
			this->s10A21->Enabled = false;
			this->s10A21->Location = System::Drawing::Point(176, 40);
			this->s10A21->Name = S"s10A21";
			this->s10A21->Size = System::Drawing::Size(64, 20);
			this->s10A21->TabIndex = 40;
			this->s10A21->Text = S"XXXX PSIA";
			// 
			// s10A18
			// 
			this->s10A18->Enabled = false;
			this->s10A18->Location = System::Drawing::Point(176, 16);
			this->s10A18->Name = S"s10A18";
			this->s10A18->Size = System::Drawing::Size(64, 20);
			this->s10A18->TabIndex = 39;
			this->s10A18->Text = S"XXXX PSIA";
			// 
			// label56
			// 
			this->label56->Location = System::Drawing::Point(240, 280);
			this->label56->Name = S"label56";
			this->label56->Size = System::Drawing::Size(136, 24);
			this->label56->TabIndex = 38;
			this->label56->Text = S"SM FU MANIF D PRESS";
			this->label56->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label57
			// 
			this->label57->Location = System::Drawing::Point(240, 256);
			this->label57->Name = S"label57";
			this->label57->Size = System::Drawing::Size(136, 24);
			this->label57->TabIndex = 37;
			this->label57->Text = S"SM OX MANIF D PRESS";
			this->label57->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label58
			// 
			this->label58->Location = System::Drawing::Point(240, 232);
			this->label58->Name = S"label58";
			this->label58->Size = System::Drawing::Size(136, 24);
			this->label58->TabIndex = 36;
			this->label58->Text = S"SM HE MANIF D PRESS";
			this->label58->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label53
			// 
			this->label53->Location = System::Drawing::Point(240, 208);
			this->label53->Name = S"label53";
			this->label53->Size = System::Drawing::Size(136, 24);
			this->label53->TabIndex = 35;
			this->label53->Text = S"SM FU MANIF C PRESS";
			this->label53->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label54
			// 
			this->label54->Location = System::Drawing::Point(240, 184);
			this->label54->Name = S"label54";
			this->label54->Size = System::Drawing::Size(136, 24);
			this->label54->TabIndex = 34;
			this->label54->Text = S"SM OX MANIF C PRESS";
			this->label54->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label55
			// 
			this->label55->Location = System::Drawing::Point(240, 160);
			this->label55->Name = S"label55";
			this->label55->Size = System::Drawing::Size(136, 24);
			this->label55->TabIndex = 33;
			this->label55->Text = S"SM HE MANIF C PRESS";
			this->label55->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label50
			// 
			this->label50->Location = System::Drawing::Point(240, 136);
			this->label50->Name = S"label50";
			this->label50->Size = System::Drawing::Size(136, 24);
			this->label50->TabIndex = 32;
			this->label50->Text = S"SM FU MANIF B PRESS";
			this->label50->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label51
			// 
			this->label51->Location = System::Drawing::Point(240, 112);
			this->label51->Name = S"label51";
			this->label51->Size = System::Drawing::Size(136, 24);
			this->label51->TabIndex = 31;
			this->label51->Text = S"SM OX MANIF B PRESS";
			this->label51->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label52
			// 
			this->label52->Location = System::Drawing::Point(240, 88);
			this->label52->Name = S"label52";
			this->label52->Size = System::Drawing::Size(136, 24);
			this->label52->TabIndex = 30;
			this->label52->Text = S"SM HE MANIF B PRESS";
			this->label52->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label49
			// 
			this->label49->Location = System::Drawing::Point(240, 64);
			this->label49->Name = S"label49";
			this->label49->Size = System::Drawing::Size(136, 24);
			this->label49->TabIndex = 29;
			this->label49->Text = S"SM FU MANIF A PRESS";
			this->label49->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label48
			// 
			this->label48->Location = System::Drawing::Point(240, 40);
			this->label48->Name = S"label48";
			this->label48->Size = System::Drawing::Size(136, 24);
			this->label48->TabIndex = 28;
			this->label48->Text = S"SM OX MANIF A PRESS";
			this->label48->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label47
			// 
			this->label47->Location = System::Drawing::Point(240, 16);
			this->label47->Name = S"label47";
			this->label47->Size = System::Drawing::Size(136, 24);
			this->label47->TabIndex = 27;
			this->label47->Text = S"SM HE MANIF A PRESS";
			this->label47->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label46
			// 
			this->label46->Location = System::Drawing::Point(8, 520);
			this->label46->Name = S"label46";
			this->label46->Size = System::Drawing::Size(168, 24);
			this->label46->TabIndex = 26;
			this->label46->Text = S"SM ENG PACKAGE D TEMP";
			this->label46->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label45
			// 
			this->label45->Location = System::Drawing::Point(8, 496);
			this->label45->Name = S"label45";
			this->label45->Size = System::Drawing::Size(168, 24);
			this->label45->TabIndex = 25;
			this->label45->Text = S"SM ENG PACKAGE C TEMP";
			this->label45->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label44
			// 
			this->label44->Location = System::Drawing::Point(8, 472);
			this->label44->Name = S"label44";
			this->label44->Size = System::Drawing::Size(168, 24);
			this->label44->TabIndex = 24;
			this->label44->Text = S"SM ENG PACKAGE B TEMP";
			this->label44->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label43
			// 
			this->label43->Location = System::Drawing::Point(8, 448);
			this->label43->Name = S"label43";
			this->label43->Size = System::Drawing::Size(168, 24);
			this->label43->TabIndex = 23;
			this->label43->Text = S"SM ENG PACKAGE A TEMP";
			this->label43->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label42
			// 
			this->label42->Location = System::Drawing::Point(8, 424);
			this->label42->Name = S"label42";
			this->label42->Size = System::Drawing::Size(168, 24);
			this->label42->TabIndex = 22;
			this->label42->Text = S"SM HE PRESS/TEMP RATIO D";
			this->label42->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label41
			// 
			this->label41->Location = System::Drawing::Point(8, 400);
			this->label41->Name = S"label41";
			this->label41->Size = System::Drawing::Size(168, 24);
			this->label41->TabIndex = 21;
			this->label41->Text = S"SM HE PRESS/TEMP RATIO C";
			this->label41->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label40
			// 
			this->label40->Location = System::Drawing::Point(8, 376);
			this->label40->Name = S"label40";
			this->label40->Size = System::Drawing::Size(168, 24);
			this->label40->TabIndex = 20;
			this->label40->Text = S"SM HE PRESS/TEMP RATIO B";
			this->label40->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label39
			// 
			this->label39->Location = System::Drawing::Point(8, 352);
			this->label39->Name = S"label39";
			this->label39->Size = System::Drawing::Size(168, 24);
			this->label39->TabIndex = 19;
			this->label39->Text = S"SM HE PRESS/TEMP RATIO A";
			this->label39->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label35
			// 
			this->label35->Location = System::Drawing::Point(8, 328);
			this->label35->Name = S"label35";
			this->label35->Size = System::Drawing::Size(168, 24);
			this->label35->TabIndex = 18;
			this->label35->Text = S"SM HE TK D TEMP";
			this->label35->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label36
			// 
			this->label36->Location = System::Drawing::Point(8, 304);
			this->label36->Name = S"label36";
			this->label36->Size = System::Drawing::Size(168, 24);
			this->label36->TabIndex = 17;
			this->label36->Text = S"SM HE TK C TEMP";
			this->label36->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label37
			// 
			this->label37->Location = System::Drawing::Point(8, 280);
			this->label37->Name = S"label37";
			this->label37->Size = System::Drawing::Size(168, 24);
			this->label37->TabIndex = 16;
			this->label37->Text = S"SM HE TK B TEMP";
			this->label37->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label38
			// 
			this->label38->Location = System::Drawing::Point(8, 256);
			this->label38->Name = S"label38";
			this->label38->Size = System::Drawing::Size(168, 24);
			this->label38->TabIndex = 15;
			this->label38->Text = S"SM HE TK A TEMP";
			this->label38->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label34
			// 
			this->label34->Location = System::Drawing::Point(8, 232);
			this->label34->Name = S"label34";
			this->label34->Size = System::Drawing::Size(168, 24);
			this->label34->TabIndex = 14;
			this->label34->Text = S"SM HE TK D PRESS";
			this->label34->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label33
			// 
			this->label33->Location = System::Drawing::Point(8, 208);
			this->label33->Name = S"label33";
			this->label33->Size = System::Drawing::Size(168, 24);
			this->label33->TabIndex = 13;
			this->label33->Text = S"SM HE TK C PRESS";
			this->label33->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label31
			// 
			this->label31->Location = System::Drawing::Point(8, 184);
			this->label31->Name = S"label31";
			this->label31->Size = System::Drawing::Size(168, 24);
			this->label31->TabIndex = 12;
			this->label31->Text = S"SM HE TK B PRESS";
			this->label31->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label32
			// 
			this->label32->Location = System::Drawing::Point(8, 160);
			this->label32->Name = S"label32";
			this->label32->Size = System::Drawing::Size(168, 24);
			this->label32->TabIndex = 11;
			this->label32->Text = S"SM HE TK A PRESS";
			this->label32->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label30
			// 
			this->label30->Location = System::Drawing::Point(8, 136);
			this->label30->Name = S"label30";
			this->label30->Size = System::Drawing::Size(168, 24);
			this->label30->TabIndex = 10;
			this->label30->Text = S"CM HE MANIF 2 PRESS";
			this->label30->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label29
			// 
			this->label29->Location = System::Drawing::Point(8, 112);
			this->label29->Name = S"label29";
			this->label29->Size = System::Drawing::Size(168, 24);
			this->label29->TabIndex = 9;
			this->label29->Text = S"CM HE MANIF 1 PRESS";
			this->label29->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label28
			// 
			this->label28->Location = System::Drawing::Point(8, 88);
			this->label28->Name = S"label28";
			this->label28->Size = System::Drawing::Size(168, 24);
			this->label28->TabIndex = 8;
			this->label28->Text = S"CM HE TK B TEMP";
			this->label28->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label27
			// 
			this->label27->Location = System::Drawing::Point(8, 64);
			this->label27->Name = S"label27";
			this->label27->Size = System::Drawing::Size(168, 24);
			this->label27->TabIndex = 7;
			this->label27->Text = S"CM HE TK A TEMP";
			this->label27->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label26
			// 
			this->label26->Location = System::Drawing::Point(8, 40);
			this->label26->Name = S"label26";
			this->label26->Size = System::Drawing::Size(168, 24);
			this->label26->TabIndex = 6;
			this->label26->Text = S"CM HE TK B PRESS";
			this->label26->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label25
			// 
			this->label25->Location = System::Drawing::Point(8, 16);
			this->label25->Name = S"label25";
			this->label25->Size = System::Drawing::Size(168, 24);
			this->label25->TabIndex = 5;
			this->label25->Text = S"CM HE TK A PRESS";
			this->label25->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// SPSForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(702, 604);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = S"SPSForm";
			this->Text = S"SERVICE PROPULSION SYSTEM / REACTION CONTROL SYSTEM";
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);

		}		


};
}