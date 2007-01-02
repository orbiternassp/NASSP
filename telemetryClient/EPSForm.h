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
	/// Summary for EPSForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	__gc class EPSForm : public System::Windows::Forms::Form
	{

	public: 
		EPSForm(void)
		{
			InitializeComponent();
			this->add_Closing(new CancelEventHandler(this,EPSForm_Closing));
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
	private: System::Void EPSForm_Closing(Object * sender, System::ComponentModel::CancelEventArgs* e){			
			e->Cancel = true; // don't close			
			this->Hide();     // Hide instead.
		}      		

	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::TextBox *  s10A141;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::TextBox *  s10A144;
	private: System::Windows::Forms::TextBox *  s10A147;
	private: System::Windows::Forms::TextBox *  s10A42;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::Label *  label6;
	private: System::Windows::Forms::Label *  label7;
	private: System::Windows::Forms::Label *  label8;
	private: System::Windows::Forms::TextBox *  s10A150;
	private: System::Windows::Forms::TextBox *  s10A66;
	private: System::Windows::Forms::TextBox *  s10A36;
	private: System::Windows::Forms::TextBox *  s10A39;
	private: System::Windows::Forms::Label *  label9;
	private: System::Windows::Forms::Label *  label10;
	private: System::Windows::Forms::Label *  label11;
	private: System::Windows::Forms::Label *  label12;
	private: System::Windows::Forms::TextBox *  s10A54;
	private: System::Windows::Forms::TextBox *  s10A57;
	private: System::Windows::Forms::TextBox *  s10A60;
	private: System::Windows::Forms::TextBox *  s10A63;
	private: System::Windows::Forms::Label *  label13;
	private: System::Windows::Forms::Label *  label14;
	private: System::Windows::Forms::Label *  label15;
	private: System::Windows::Forms::TextBox *  s10A86;
	private: System::Windows::Forms::TextBox *  s10A88;
	private: System::Windows::Forms::TextBox *  s10A89;
	private: System::Windows::Forms::Label *  label16;
	private: System::Windows::Forms::Label *  label17;
	private: System::Windows::Forms::Label *  label18;
	private: System::Windows::Forms::Label *  label19;
	private: System::Windows::Forms::TextBox *  s11A147;
	private: System::Windows::Forms::TextBox *  s11A56;
	private: System::Windows::Forms::TextBox *  s11A57;
	private: System::Windows::Forms::TextBox *  s11A58;
	private: System::Windows::Forms::Label *  label20;
	private: System::Windows::Forms::Label *  label21;
	private: System::Windows::Forms::Label *  label22;
	private: System::Windows::Forms::Label *  label23;
	private: System::Windows::Forms::Label *  label24;
	private: System::Windows::Forms::Label *  label25;
	private: System::Windows::Forms::Label *  label26;



	public: System::Windows::Forms::TextBox *  s11A91;
	public: System::Windows::Forms::TextBox *  s11A93;
	private: System::Windows::Forms::TextBox *  s11A75;
	private: System::Windows::Forms::TextBox *  s11A73;
	private: System::Windows::Forms::TextBox *  s11A74;
	public: System::Windows::Forms::TextBox *  s11A109;
	private: System::Windows::Forms::TextBox *  s11A110;






	private: System::Windows::Forms::Label *  label30;
	private: System::Windows::Forms::Label *  label31;
	private: System::Windows::Forms::Label *  label32;



	private: System::Windows::Forms::Label *  label33;
	private: System::Windows::Forms::Label *  label34;
	private: System::Windows::Forms::Label *  label35;
	private: System::Windows::Forms::TextBox *  s11A69;
	private: System::Windows::Forms::TextBox *  s11A68;
	private: System::Windows::Forms::TextBox *  s11A67;
	private: System::Windows::Forms::TextBox *  s11A72;
	private: System::Windows::Forms::TextBox *  s11A71;
	private: System::Windows::Forms::TextBox *  s11A70;
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
private: System::Windows::Forms::Label *  label59;
private: System::Windows::Forms::Label *  label60;
private: System::Windows::Forms::TextBox *  s11E281;
private: System::Windows::Forms::TextBox *  s11E282;
private: System::Windows::Forms::TextBox *  s11E283;
private: System::Windows::Forms::TextBox *  s11A47;
private: System::Windows::Forms::TextBox *  s10A51;
private: System::Windows::Forms::TextBox *  s10A123;
private: System::Windows::Forms::TextBox *  s10A69;
private: System::Windows::Forms::TextBox *  s10A72;
private: System::Windows::Forms::TextBox *  s10A75;
private: System::Windows::Forms::TextBox *  s10A129;
private: System::Windows::Forms::TextBox *  s10A126;
private: System::Windows::Forms::TextBox *  s10A78;
private: System::Windows::Forms::TextBox *  s10A131;
private: System::Windows::Forms::TextBox *  s10A130;
private: System::Windows::Forms::TextBox *  s10A132;
private: System::Windows::Forms::TextBox *  s10A67;


private: System::Windows::Forms::TextBox *  s11A76;
private: System::Windows::Forms::TextBox *  s11A85;
private: System::Windows::Forms::TextBox *  s11A84;
private: System::Windows::Forms::TextBox *  s11A77;
private: System::Windows::Forms::TextBox *  s11A78;
private: System::Windows::Forms::TextBox *  s11A79;
private: System::Windows::Forms::TextBox *  s11A80;
private: System::Windows::Forms::TextBox *  s11A81;
private: System::Windows::Forms::TextBox *  s11A82;
private: System::Windows::Forms::TextBox *  s11A35;
private: System::Windows::Forms::TextBox *  s11A30;
private: System::Windows::Forms::TextBox *  s11A29;
private: System::Windows::Forms::Label *  label29;
private: System::Windows::Forms::Label *  label28;
private: System::Windows::Forms::Label *  label27;



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
			this->s10A141 = new System::Windows::Forms::TextBox();
			this->label1 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->label3 = new System::Windows::Forms::Label();
			this->label4 = new System::Windows::Forms::Label();
			this->s10A144 = new System::Windows::Forms::TextBox();
			this->s10A147 = new System::Windows::Forms::TextBox();
			this->s10A42 = new System::Windows::Forms::TextBox();
			this->label5 = new System::Windows::Forms::Label();
			this->label6 = new System::Windows::Forms::Label();
			this->label7 = new System::Windows::Forms::Label();
			this->label8 = new System::Windows::Forms::Label();
			this->s10A150 = new System::Windows::Forms::TextBox();
			this->s10A66 = new System::Windows::Forms::TextBox();
			this->s10A36 = new System::Windows::Forms::TextBox();
			this->s10A39 = new System::Windows::Forms::TextBox();
			this->label9 = new System::Windows::Forms::Label();
			this->label10 = new System::Windows::Forms::Label();
			this->label11 = new System::Windows::Forms::Label();
			this->label12 = new System::Windows::Forms::Label();
			this->s10A54 = new System::Windows::Forms::TextBox();
			this->s10A57 = new System::Windows::Forms::TextBox();
			this->s10A60 = new System::Windows::Forms::TextBox();
			this->s10A63 = new System::Windows::Forms::TextBox();
			this->label13 = new System::Windows::Forms::Label();
			this->label14 = new System::Windows::Forms::Label();
			this->label15 = new System::Windows::Forms::Label();
			this->s10A86 = new System::Windows::Forms::TextBox();
			this->s10A88 = new System::Windows::Forms::TextBox();
			this->s10A89 = new System::Windows::Forms::TextBox();
			this->label16 = new System::Windows::Forms::Label();
			this->label17 = new System::Windows::Forms::Label();
			this->label18 = new System::Windows::Forms::Label();
			this->label19 = new System::Windows::Forms::Label();
			this->s11A147 = new System::Windows::Forms::TextBox();
			this->s11A56 = new System::Windows::Forms::TextBox();
			this->s11A57 = new System::Windows::Forms::TextBox();
			this->s11A58 = new System::Windows::Forms::TextBox();
			this->label20 = new System::Windows::Forms::Label();
			this->label21 = new System::Windows::Forms::Label();
			this->label22 = new System::Windows::Forms::Label();
			this->label23 = new System::Windows::Forms::Label();
			this->label24 = new System::Windows::Forms::Label();
			this->label25 = new System::Windows::Forms::Label();
			this->label26 = new System::Windows::Forms::Label();
			this->s11A91 = new System::Windows::Forms::TextBox();
			this->s11A93 = new System::Windows::Forms::TextBox();
			this->s11A75 = new System::Windows::Forms::TextBox();
			this->s11A73 = new System::Windows::Forms::TextBox();
			this->s11A74 = new System::Windows::Forms::TextBox();
			this->s11A109 = new System::Windows::Forms::TextBox();
			this->s11A110 = new System::Windows::Forms::TextBox();
			this->s11A69 = new System::Windows::Forms::TextBox();
			this->s11A68 = new System::Windows::Forms::TextBox();
			this->s11A67 = new System::Windows::Forms::TextBox();
			this->label30 = new System::Windows::Forms::Label();
			this->label31 = new System::Windows::Forms::Label();
			this->label32 = new System::Windows::Forms::Label();
			this->s11A72 = new System::Windows::Forms::TextBox();
			this->s11A71 = new System::Windows::Forms::TextBox();
			this->s11A70 = new System::Windows::Forms::TextBox();
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
			this->label59 = new System::Windows::Forms::Label();
			this->label60 = new System::Windows::Forms::Label();
			this->s11E281 = new System::Windows::Forms::TextBox();
			this->s11E282 = new System::Windows::Forms::TextBox();
			this->s11E283 = new System::Windows::Forms::TextBox();
			this->s11A47 = new System::Windows::Forms::TextBox();
			this->s10A51 = new System::Windows::Forms::TextBox();
			this->s10A123 = new System::Windows::Forms::TextBox();
			this->s10A69 = new System::Windows::Forms::TextBox();
			this->s10A72 = new System::Windows::Forms::TextBox();
			this->s10A75 = new System::Windows::Forms::TextBox();
			this->s10A129 = new System::Windows::Forms::TextBox();
			this->s10A126 = new System::Windows::Forms::TextBox();
			this->s10A78 = new System::Windows::Forms::TextBox();
			this->s10A131 = new System::Windows::Forms::TextBox();
			this->s10A130 = new System::Windows::Forms::TextBox();
			this->s10A132 = new System::Windows::Forms::TextBox();
			this->s10A67 = new System::Windows::Forms::TextBox();
			this->s11A85 = new System::Windows::Forms::TextBox();
			this->s11A84 = new System::Windows::Forms::TextBox();
			this->s11A76 = new System::Windows::Forms::TextBox();
			this->s11A77 = new System::Windows::Forms::TextBox();
			this->s11A78 = new System::Windows::Forms::TextBox();
			this->s11A79 = new System::Windows::Forms::TextBox();
			this->s11A80 = new System::Windows::Forms::TextBox();
			this->s11A81 = new System::Windows::Forms::TextBox();
			this->s11A82 = new System::Windows::Forms::TextBox();
			this->s11A35 = new System::Windows::Forms::TextBox();
			this->s11A30 = new System::Windows::Forms::TextBox();
			this->s11A29 = new System::Windows::Forms::TextBox();
			this->label29 = new System::Windows::Forms::Label();
			this->label28 = new System::Windows::Forms::Label();
			this->label27 = new System::Windows::Forms::Label();
			this->SuspendLayout();
			// 
			// s10A141
			// 
			this->s10A141->Enabled = false;
			this->s10A141->Location = System::Drawing::Point(128, 8);
			this->s10A141->Name = S"s10A141";
			this->s10A141->Size = System::Drawing::Size(64, 20);
			this->s10A141->TabIndex = 4;
			this->s10A141->Text = S"XXX.XX%";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 8);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(120, 24);
			this->label1->TabIndex = 3;
			this->label1->Text = S"H2 TK 1 QTY";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 32);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(120, 24);
			this->label2->TabIndex = 5;
			this->label2->Text = S"H2 TK 2 QTY";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 56);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(120, 24);
			this->label3->TabIndex = 6;
			this->label3->Text = S"O2 TK 1 QTY";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(8, 80);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(120, 24);
			this->label4->TabIndex = 7;
			this->label4->Text = S"O2 TK 2 QTY";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A144
			// 
			this->s10A144->Enabled = false;
			this->s10A144->Location = System::Drawing::Point(128, 32);
			this->s10A144->Name = S"s10A144";
			this->s10A144->Size = System::Drawing::Size(64, 20);
			this->s10A144->TabIndex = 8;
			this->s10A144->Text = S"XXX.XX%";
			// 
			// s10A147
			// 
			this->s10A147->Enabled = false;
			this->s10A147->Location = System::Drawing::Point(128, 56);
			this->s10A147->Name = S"s10A147";
			this->s10A147->Size = System::Drawing::Size(64, 20);
			this->s10A147->TabIndex = 9;
			this->s10A147->Text = S"XXX.XX%";
			// 
			// s10A42
			// 
			this->s10A42->Enabled = false;
			this->s10A42->Location = System::Drawing::Point(128, 80);
			this->s10A42->Name = S"s10A42";
			this->s10A42->Size = System::Drawing::Size(64, 20);
			this->s10A42->TabIndex = 10;
			this->s10A42->Text = S"XXX.XX%";
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(8, 128);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(120, 24);
			this->label5->TabIndex = 14;
			this->label5->Text = S"O2 TK 2 PRESS";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(8, 104);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(120, 24);
			this->label6->TabIndex = 13;
			this->label6->Text = S"O2 TK 1 PRESS";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(8, 176);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(120, 24);
			this->label7->TabIndex = 16;
			this->label7->Text = S"H2 TK 2 PRESS";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(8, 152);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(120, 24);
			this->label8->TabIndex = 15;
			this->label8->Text = S"H2 TK 1 PRESS";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A150
			// 
			this->s10A150->Enabled = false;
			this->s10A150->Location = System::Drawing::Point(128, 104);
			this->s10A150->Name = S"s10A150";
			this->s10A150->Size = System::Drawing::Size(64, 20);
			this->s10A150->TabIndex = 17;
			this->s10A150->Text = S"XXXX PSIA";
			// 
			// s10A66
			// 
			this->s10A66->Enabled = false;
			this->s10A66->Location = System::Drawing::Point(128, 128);
			this->s10A66->Name = S"s10A66";
			this->s10A66->Size = System::Drawing::Size(64, 20);
			this->s10A66->TabIndex = 18;
			this->s10A66->Text = S"XXXX PSIA";
			// 
			// s10A36
			// 
			this->s10A36->Enabled = false;
			this->s10A36->Location = System::Drawing::Point(128, 152);
			this->s10A36->Name = S"s10A36";
			this->s10A36->Size = System::Drawing::Size(64, 20);
			this->s10A36->TabIndex = 19;
			this->s10A36->Text = S"XXXX PSIA";
			// 
			// s10A39
			// 
			this->s10A39->Enabled = false;
			this->s10A39->Location = System::Drawing::Point(128, 176);
			this->s10A39->Name = S"s10A39";
			this->s10A39->Size = System::Drawing::Size(64, 20);
			this->s10A39->TabIndex = 20;
			this->s10A39->Text = S"XXXX PSIA";
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(8, 272);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(120, 24);
			this->label9->TabIndex = 24;
			this->label9->Text = S"O2 TK 2 TEMP";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 248);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(120, 24);
			this->label10->TabIndex = 23;
			this->label10->Text = S"O2 TK 1 TEMP";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(8, 224);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(120, 24);
			this->label11->TabIndex = 22;
			this->label11->Text = S"H2 TK 2 TEMP";
			this->label11->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label12
			// 
			this->label12->Location = System::Drawing::Point(8, 200);
			this->label12->Name = S"label12";
			this->label12->Size = System::Drawing::Size(120, 24);
			this->label12->TabIndex = 21;
			this->label12->Text = S"H2 TK 1 TEMP";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A54
			// 
			this->s10A54->Enabled = false;
			this->s10A54->Location = System::Drawing::Point(128, 200);
			this->s10A54->Name = S"s10A54";
			this->s10A54->Size = System::Drawing::Size(64, 20);
			this->s10A54->TabIndex = 25;
			this->s10A54->Text = S"XXX.XX °F";
			// 
			// s10A57
			// 
			this->s10A57->Enabled = false;
			this->s10A57->Location = System::Drawing::Point(128, 224);
			this->s10A57->Name = S"s10A57";
			this->s10A57->Size = System::Drawing::Size(64, 20);
			this->s10A57->TabIndex = 26;
			this->s10A57->Text = S"XXX.XX °F";
			// 
			// s10A60
			// 
			this->s10A60->Enabled = false;
			this->s10A60->Location = System::Drawing::Point(128, 248);
			this->s10A60->Name = S"s10A60";
			this->s10A60->Size = System::Drawing::Size(64, 20);
			this->s10A60->TabIndex = 27;
			this->s10A60->Text = S"XXX.XX °F";
			// 
			// s10A63
			// 
			this->s10A63->Enabled = false;
			this->s10A63->Location = System::Drawing::Point(128, 272);
			this->s10A63->Name = S"s10A63";
			this->s10A63->Size = System::Drawing::Size(64, 20);
			this->s10A63->TabIndex = 28;
			this->s10A63->Text = S"XXX.XX °F";
			// 
			// label13
			// 
			this->label13->Location = System::Drawing::Point(8, 296);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(120, 24);
			this->label13->TabIndex = 29;
			this->label13->Text = S"INVERTER 1 TEMP";
			this->label13->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label14
			// 
			this->label14->Location = System::Drawing::Point(8, 320);
			this->label14->Name = S"label14";
			this->label14->Size = System::Drawing::Size(120, 24);
			this->label14->TabIndex = 30;
			this->label14->Text = S"INVERTER 2 TEMP";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label15
			// 
			this->label15->Location = System::Drawing::Point(8, 344);
			this->label15->Name = S"label15";
			this->label15->Size = System::Drawing::Size(120, 24);
			this->label15->TabIndex = 31;
			this->label15->Text = S"INVERTER 3 TEMP";
			this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A86
			// 
			this->s10A86->Enabled = false;
			this->s10A86->Location = System::Drawing::Point(128, 296);
			this->s10A86->Name = S"s10A86";
			this->s10A86->Size = System::Drawing::Size(64, 20);
			this->s10A86->TabIndex = 32;
			this->s10A86->Text = S"XXX.XX °F";
			// 
			// s10A88
			// 
			this->s10A88->Enabled = false;
			this->s10A88->Location = System::Drawing::Point(128, 320);
			this->s10A88->Name = S"s10A88";
			this->s10A88->Size = System::Drawing::Size(64, 20);
			this->s10A88->TabIndex = 33;
			this->s10A88->Text = S"XXX.XX °F";
			// 
			// s10A89
			// 
			this->s10A89->Enabled = false;
			this->s10A89->Location = System::Drawing::Point(128, 344);
			this->s10A89->Name = S"s10A89";
			this->s10A89->Size = System::Drawing::Size(64, 20);
			this->s10A89->TabIndex = 34;
			this->s10A89->Text = S"XXX.XX °F";
			// 
			// label16
			// 
			this->label16->Location = System::Drawing::Point(8, 368);
			this->label16->Name = S"label16";
			this->label16->Size = System::Drawing::Size(120, 24);
			this->label16->TabIndex = 35;
			this->label16->Text = S"AC BUS 1 FA VOLTS";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label17
			// 
			this->label17->Location = System::Drawing::Point(8, 392);
			this->label17->Name = S"label17";
			this->label17->Size = System::Drawing::Size(120, 24);
			this->label17->TabIndex = 36;
			this->label17->Text = S"AC BUS 2 FA VOLTS";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label18
			// 
			this->label18->Location = System::Drawing::Point(8, 416);
			this->label18->Name = S"label18";
			this->label18->Size = System::Drawing::Size(120, 24);
			this->label18->TabIndex = 37;
			this->label18->Text = S"MAIN BUS A VOLTS";
			this->label18->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label19
			// 
			this->label19->Location = System::Drawing::Point(8, 440);
			this->label19->Name = S"label19";
			this->label19->Size = System::Drawing::Size(120, 24);
			this->label19->TabIndex = 38;
			this->label19->Text = S"MAIN BUS B VOLTS";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s11A147
			// 
			this->s11A147->Enabled = false;
			this->s11A147->Location = System::Drawing::Point(128, 368);
			this->s11A147->Name = S"s11A147";
			this->s11A147->Size = System::Drawing::Size(64, 20);
			this->s11A147->TabIndex = 39;
			this->s11A147->Text = S"XXX.XX V";
			// 
			// s11A56
			// 
			this->s11A56->Enabled = false;
			this->s11A56->Location = System::Drawing::Point(128, 392);
			this->s11A56->Name = S"s11A56";
			this->s11A56->Size = System::Drawing::Size(64, 20);
			this->s11A56->TabIndex = 40;
			this->s11A56->Text = S"XXX.XX V";
			// 
			// s11A57
			// 
			this->s11A57->Enabled = false;
			this->s11A57->Location = System::Drawing::Point(128, 416);
			this->s11A57->Name = S"s11A57";
			this->s11A57->Size = System::Drawing::Size(64, 20);
			this->s11A57->TabIndex = 41;
			this->s11A57->Text = S"XX.XX V";
			// 
			// s11A58
			// 
			this->s11A58->Enabled = false;
			this->s11A58->Location = System::Drawing::Point(128, 440);
			this->s11A58->Name = S"s11A58";
			this->s11A58->Size = System::Drawing::Size(64, 20);
			this->s11A58->TabIndex = 42;
			this->s11A58->Text = S"XX.XX V";
			// 
			// label20
			// 
			this->label20->Location = System::Drawing::Point(8, 464);
			this->label20->Name = S"label20";
			this->label20->Size = System::Drawing::Size(120, 24);
			this->label20->TabIndex = 43;
			this->label20->Text = S"BAT BUS A VOLTS";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label21
			// 
			this->label21->Location = System::Drawing::Point(8, 488);
			this->label21->Name = S"label21";
			this->label21->Size = System::Drawing::Size(120, 24);
			this->label21->TabIndex = 44;
			this->label21->Text = S"BAT BUS B VOLTS";
			this->label21->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label22
			// 
			this->label22->Location = System::Drawing::Point(392, 464);
			this->label22->Name = S"label22";
			this->label22->Size = System::Drawing::Size(120, 24);
			this->label22->TabIndex = 45;
			this->label22->Text = S"BAT CHRGR AMPS";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label23
			// 
			this->label23->Location = System::Drawing::Point(392, 368);
			this->label23->Name = S"label23";
			this->label23->Size = System::Drawing::Size(120, 24);
			this->label23->TabIndex = 46;
			this->label23->Text = S"BAT A CUR";
			this->label23->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label24
			// 
			this->label24->Location = System::Drawing::Point(392, 392);
			this->label24->Name = S"label24";
			this->label24->Size = System::Drawing::Size(120, 24);
			this->label24->TabIndex = 47;
			this->label24->Text = S"BAT B CUR";
			this->label24->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label25
			// 
			this->label25->Location = System::Drawing::Point(392, 416);
			this->label25->Name = S"label25";
			this->label25->Size = System::Drawing::Size(120, 24);
			this->label25->TabIndex = 48;
			this->label25->Text = S"BAT C CUR";
			this->label25->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label26
			// 
			this->label26->Location = System::Drawing::Point(200, 440);
			this->label26->Name = S"label26";
			this->label26->Size = System::Drawing::Size(120, 24);
			this->label26->TabIndex = 49;
			this->label26->Text = S"BAT RLY BUS VOLTS";
			this->label26->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s11A91
			// 
			this->s11A91->Enabled = false;
			this->s11A91->Location = System::Drawing::Point(128, 464);
			this->s11A91->Name = S"s11A91";
			this->s11A91->Size = System::Drawing::Size(64, 20);
			this->s11A91->TabIndex = 53;
			this->s11A91->Text = S"XX.XX V";
			// 
			// s11A93
			// 
			this->s11A93->Enabled = false;
			this->s11A93->Location = System::Drawing::Point(128, 488);
			this->s11A93->Name = S"s11A93";
			this->s11A93->Size = System::Drawing::Size(64, 20);
			this->s11A93->TabIndex = 54;
			this->s11A93->Text = S"XX.XX V";
			// 
			// s11A75
			// 
			this->s11A75->Enabled = false;
			this->s11A75->Location = System::Drawing::Point(320, 440);
			this->s11A75->Name = S"s11A75";
			this->s11A75->Size = System::Drawing::Size(64, 20);
			this->s11A75->TabIndex = 55;
			this->s11A75->Text = S"XX.XX V";
			// 
			// s11A73
			// 
			this->s11A73->Enabled = false;
			this->s11A73->Location = System::Drawing::Point(512, 464);
			this->s11A73->Name = S"s11A73";
			this->s11A73->Size = System::Drawing::Size(64, 20);
			this->s11A73->TabIndex = 56;
			this->s11A73->Text = S"XXX.XX A";
			// 
			// s11A74
			// 
			this->s11A74->Enabled = false;
			this->s11A74->Location = System::Drawing::Point(512, 368);
			this->s11A74->Name = S"s11A74";
			this->s11A74->Size = System::Drawing::Size(64, 20);
			this->s11A74->TabIndex = 57;
			this->s11A74->Text = S"XXX.XX A";
			// 
			// s11A109
			// 
			this->s11A109->Enabled = false;
			this->s11A109->Location = System::Drawing::Point(512, 392);
			this->s11A109->Name = S"s11A109";
			this->s11A109->Size = System::Drawing::Size(64, 20);
			this->s11A109->TabIndex = 58;
			this->s11A109->Text = S"XXX.XX A";
			// 
			// s11A110
			// 
			this->s11A110->Enabled = false;
			this->s11A110->Location = System::Drawing::Point(512, 416);
			this->s11A110->Name = S"s11A110";
			this->s11A110->Size = System::Drawing::Size(64, 20);
			this->s11A110->TabIndex = 59;
			this->s11A110->Text = S"XXX.XX A";
			// 
			// s11A69
			// 
			this->s11A69->Enabled = false;
			this->s11A69->Location = System::Drawing::Point(320, 56);
			this->s11A69->Name = S"s11A69";
			this->s11A69->Size = System::Drawing::Size(64, 20);
			this->s11A69->TabIndex = 68;
			this->s11A69->Text = S"XXXX PSIA";
			// 
			// s11A68
			// 
			this->s11A68->Enabled = false;
			this->s11A68->Location = System::Drawing::Point(320, 32);
			this->s11A68->Name = S"s11A68";
			this->s11A68->Size = System::Drawing::Size(64, 20);
			this->s11A68->TabIndex = 67;
			this->s11A68->Text = S"XXXX PSIA";
			// 
			// s11A67
			// 
			this->s11A67->Enabled = false;
			this->s11A67->Location = System::Drawing::Point(320, 8);
			this->s11A67->Name = S"s11A67";
			this->s11A67->Size = System::Drawing::Size(64, 20);
			this->s11A67->TabIndex = 66;
			this->s11A67->Text = S"XXXX PSIA";
			// 
			// label30
			// 
			this->label30->Location = System::Drawing::Point(200, 56);
			this->label30->Name = S"label30";
			this->label30->Size = System::Drawing::Size(120, 24);
			this->label30->TabIndex = 65;
			this->label30->Text = S"FC 3 O2 PRESS";
			this->label30->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label31
			// 
			this->label31->Location = System::Drawing::Point(200, 32);
			this->label31->Name = S"label31";
			this->label31->Size = System::Drawing::Size(120, 24);
			this->label31->TabIndex = 64;
			this->label31->Text = S"FC 2 O2 PRESS";
			this->label31->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label32
			// 
			this->label32->Location = System::Drawing::Point(200, 8);
			this->label32->Name = S"label32";
			this->label32->Size = System::Drawing::Size(120, 24);
			this->label32->TabIndex = 63;
			this->label32->Text = S"FC 1 O2 PRESS";
			this->label32->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s11A72
			// 
			this->s11A72->Enabled = false;
			this->s11A72->Location = System::Drawing::Point(320, 128);
			this->s11A72->Name = S"s11A72";
			this->s11A72->Size = System::Drawing::Size(64, 20);
			this->s11A72->TabIndex = 74;
			this->s11A72->Text = S"XXXX PSIA";
			// 
			// s11A71
			// 
			this->s11A71->Enabled = false;
			this->s11A71->Location = System::Drawing::Point(320, 104);
			this->s11A71->Name = S"s11A71";
			this->s11A71->Size = System::Drawing::Size(64, 20);
			this->s11A71->TabIndex = 73;
			this->s11A71->Text = S"XXXX PSIA";
			// 
			// s11A70
			// 
			this->s11A70->Enabled = false;
			this->s11A70->Location = System::Drawing::Point(320, 80);
			this->s11A70->Name = S"s11A70";
			this->s11A70->Size = System::Drawing::Size(64, 20);
			this->s11A70->TabIndex = 72;
			this->s11A70->Text = S"XXXX PSIA";
			// 
			// label33
			// 
			this->label33->Location = System::Drawing::Point(200, 128);
			this->label33->Name = S"label33";
			this->label33->Size = System::Drawing::Size(120, 24);
			this->label33->TabIndex = 71;
			this->label33->Text = S"FC 3 H2 PRESS";
			this->label33->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label34
			// 
			this->label34->Location = System::Drawing::Point(200, 104);
			this->label34->Name = S"label34";
			this->label34->Size = System::Drawing::Size(120, 24);
			this->label34->TabIndex = 70;
			this->label34->Text = S"FC 2 H2 PRESS";
			this->label34->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label35
			// 
			this->label35->Location = System::Drawing::Point(200, 80);
			this->label35->Name = S"label35";
			this->label35->Size = System::Drawing::Size(120, 24);
			this->label35->TabIndex = 69;
			this->label35->Text = S"FC 1 H2 PRESS";
			this->label35->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label36
			// 
			this->label36->Location = System::Drawing::Point(392, 272);
			this->label36->Name = S"label36";
			this->label36->Size = System::Drawing::Size(120, 24);
			this->label36->TabIndex = 77;
			this->label36->Text = S"FC 3 CON EXH TEMP";
			this->label36->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label37
			// 
			this->label37->Location = System::Drawing::Point(392, 248);
			this->label37->Name = S"label37";
			this->label37->Size = System::Drawing::Size(120, 24);
			this->label37->TabIndex = 76;
			this->label37->Text = S"FC 2 CON EXH TEMP";
			this->label37->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label38
			// 
			this->label38->Location = System::Drawing::Point(392, 224);
			this->label38->Name = S"label38";
			this->label38->Size = System::Drawing::Size(120, 24);
			this->label38->TabIndex = 75;
			this->label38->Text = S"FC 1 CON EXH TEMP";
			this->label38->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label39
			// 
			this->label39->Location = System::Drawing::Point(200, 272);
			this->label39->Name = S"label39";
			this->label39->Size = System::Drawing::Size(120, 24);
			this->label39->TabIndex = 80;
			this->label39->Text = S"FC 3 SKIN TEMP";
			this->label39->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label40
			// 
			this->label40->Location = System::Drawing::Point(200, 248);
			this->label40->Name = S"label40";
			this->label40->Size = System::Drawing::Size(120, 24);
			this->label40->TabIndex = 79;
			this->label40->Text = S"FC 2 SKIN TEMP";
			this->label40->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label41
			// 
			this->label41->Location = System::Drawing::Point(200, 224);
			this->label41->Name = S"label41";
			this->label41->Size = System::Drawing::Size(120, 24);
			this->label41->TabIndex = 78;
			this->label41->Text = S"FC 1 SKIN TEMP";
			this->label41->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label42
			// 
			this->label42->Location = System::Drawing::Point(392, 344);
			this->label42->Name = S"label42";
			this->label42->Size = System::Drawing::Size(120, 24);
			this->label42->TabIndex = 83;
			this->label42->Text = S"FC 3 RAD OUT TEMP";
			this->label42->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label43
			// 
			this->label43->Location = System::Drawing::Point(392, 320);
			this->label43->Name = S"label43";
			this->label43->Size = System::Drawing::Size(120, 24);
			this->label43->TabIndex = 82;
			this->label43->Text = S"FC 2 RAD OUT TEMP";
			this->label43->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label44
			// 
			this->label44->Location = System::Drawing::Point(392, 296);
			this->label44->Name = S"label44";
			this->label44->Size = System::Drawing::Size(120, 24);
			this->label44->TabIndex = 81;
			this->label44->Text = S"FC 1 RAD OUT TEMP";
			this->label44->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label45
			// 
			this->label45->Location = System::Drawing::Point(200, 344);
			this->label45->Name = S"label45";
			this->label45->Size = System::Drawing::Size(120, 24);
			this->label45->TabIndex = 86;
			this->label45->Text = S"FC 3 RAD IN TEMP";
			this->label45->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label46
			// 
			this->label46->Location = System::Drawing::Point(200, 320);
			this->label46->Name = S"label46";
			this->label46->Size = System::Drawing::Size(120, 24);
			this->label46->TabIndex = 85;
			this->label46->Text = S"FC 2 RAD IN TEMP";
			this->label46->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label47
			// 
			this->label47->Location = System::Drawing::Point(200, 296);
			this->label47->Name = S"label47";
			this->label47->Size = System::Drawing::Size(120, 24);
			this->label47->TabIndex = 84;
			this->label47->Text = S"FC 1 RAD IN TEMP";
			this->label47->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label48
			// 
			this->label48->Location = System::Drawing::Point(200, 416);
			this->label48->Name = S"label48";
			this->label48->Size = System::Drawing::Size(120, 24);
			this->label48->TabIndex = 89;
			this->label48->Text = S"FC 3 CUR";
			this->label48->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label49
			// 
			this->label49->Location = System::Drawing::Point(200, 392);
			this->label49->Name = S"label49";
			this->label49->Size = System::Drawing::Size(120, 24);
			this->label49->TabIndex = 88;
			this->label49->Text = S"FC 2 CUR";
			this->label49->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label50
			// 
			this->label50->Location = System::Drawing::Point(200, 368);
			this->label50->Name = S"label50";
			this->label50->Size = System::Drawing::Size(120, 24);
			this->label50->TabIndex = 87;
			this->label50->Text = S"FC 1 CUR";
			this->label50->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label51
			// 
			this->label51->Location = System::Drawing::Point(200, 200);
			this->label51->Name = S"label51";
			this->label51->Size = System::Drawing::Size(120, 24);
			this->label51->TabIndex = 92;
			this->label51->Text = S"FC 3 H2 FLOW";
			this->label51->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label52
			// 
			this->label52->Location = System::Drawing::Point(200, 176);
			this->label52->Name = S"label52";
			this->label52->Size = System::Drawing::Size(120, 24);
			this->label52->TabIndex = 91;
			this->label52->Text = S"FC 2 H2 FLOW";
			this->label52->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label53
			// 
			this->label53->Location = System::Drawing::Point(200, 152);
			this->label53->Name = S"label53";
			this->label53->Size = System::Drawing::Size(120, 24);
			this->label53->TabIndex = 90;
			this->label53->Text = S"FC 1 H2 FLOW";
			this->label53->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label54
			// 
			this->label54->Location = System::Drawing::Point(392, 200);
			this->label54->Name = S"label54";
			this->label54->Size = System::Drawing::Size(120, 24);
			this->label54->TabIndex = 95;
			this->label54->Text = S"FC 3 O2 FLOW";
			this->label54->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label55
			// 
			this->label55->Location = System::Drawing::Point(392, 176);
			this->label55->Name = S"label55";
			this->label55->Size = System::Drawing::Size(120, 24);
			this->label55->TabIndex = 94;
			this->label55->Text = S"FC 2 O2 FLOW";
			this->label55->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label56
			// 
			this->label56->Location = System::Drawing::Point(392, 152);
			this->label56->Name = S"label56";
			this->label56->Size = System::Drawing::Size(120, 24);
			this->label56->TabIndex = 93;
			this->label56->Text = S"FC 1 O2 FLOW";
			this->label56->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label57
			// 
			this->label57->Location = System::Drawing::Point(392, 80);
			this->label57->Name = S"label57";
			this->label57->Size = System::Drawing::Size(120, 24);
			this->label57->TabIndex = 96;
			this->label57->Text = S"FC 1 PH";
			this->label57->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label58
			// 
			this->label58->Location = System::Drawing::Point(392, 104);
			this->label58->Name = S"label58";
			this->label58->Size = System::Drawing::Size(120, 24);
			this->label58->TabIndex = 97;
			this->label58->Text = S"FC 2 PH";
			this->label58->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label59
			// 
			this->label59->Location = System::Drawing::Point(392, 128);
			this->label59->Name = S"label59";
			this->label59->Size = System::Drawing::Size(120, 24);
			this->label59->TabIndex = 98;
			this->label59->Text = S"FC 3 PH";
			this->label59->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label60
			// 
			this->label60->Location = System::Drawing::Point(392, 440);
			this->label60->Name = S"label60";
			this->label60->Size = System::Drawing::Size(120, 24);
			this->label60->TabIndex = 99;
			this->label60->Text = S"LM HEATER CUR";
			this->label60->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s11E281
			// 
			this->s11E281->Enabled = false;
			this->s11E281->Location = System::Drawing::Point(512, 80);
			this->s11E281->Name = S"s11E281";
			this->s11E281->Size = System::Drawing::Size(64, 20);
			this->s11E281->TabIndex = 100;
			this->s11E281->Text = S"XXXX";
			// 
			// s11E282
			// 
			this->s11E282->Enabled = false;
			this->s11E282->Location = System::Drawing::Point(512, 104);
			this->s11E282->Name = S"s11E282";
			this->s11E282->Size = System::Drawing::Size(64, 20);
			this->s11E282->TabIndex = 101;
			this->s11E282->Text = S"XXXX";
			// 
			// s11E283
			// 
			this->s11E283->Enabled = false;
			this->s11E283->Location = System::Drawing::Point(512, 128);
			this->s11E283->Name = S"s11E283";
			this->s11E283->Size = System::Drawing::Size(64, 20);
			this->s11E283->TabIndex = 102;
			this->s11E283->Text = S"XXXX";
			// 
			// s11A47
			// 
			this->s11A47->Enabled = false;
			this->s11A47->Location = System::Drawing::Point(512, 440);
			this->s11A47->Name = S"s11A47";
			this->s11A47->Size = System::Drawing::Size(64, 20);
			this->s11A47->TabIndex = 103;
			this->s11A47->Text = S"XXX.XX A";
			// 
			// s10A51
			// 
			this->s10A51->Enabled = false;
			this->s10A51->Location = System::Drawing::Point(512, 224);
			this->s10A51->Name = S"s10A51";
			this->s10A51->Size = System::Drawing::Size(64, 20);
			this->s10A51->TabIndex = 104;
			this->s10A51->Text = S"XXX.XX °F";
			// 
			// s10A123
			// 
			this->s10A123->Enabled = false;
			this->s10A123->Location = System::Drawing::Point(512, 248);
			this->s10A123->Name = S"s10A123";
			this->s10A123->Size = System::Drawing::Size(64, 20);
			this->s10A123->TabIndex = 105;
			this->s10A123->Text = S"XXX.XX °F";
			// 
			// s10A69
			// 
			this->s10A69->Enabled = false;
			this->s10A69->Location = System::Drawing::Point(512, 272);
			this->s10A69->Name = S"s10A69";
			this->s10A69->Size = System::Drawing::Size(64, 20);
			this->s10A69->TabIndex = 106;
			this->s10A69->Text = S"XXX.XX °F";
			// 
			// s10A72
			// 
			this->s10A72->Enabled = false;
			this->s10A72->Location = System::Drawing::Point(320, 224);
			this->s10A72->Name = S"s10A72";
			this->s10A72->Size = System::Drawing::Size(64, 20);
			this->s10A72->TabIndex = 107;
			this->s10A72->Text = S"XXX.XX °F";
			// 
			// s10A75
			// 
			this->s10A75->Enabled = false;
			this->s10A75->Location = System::Drawing::Point(320, 248);
			this->s10A75->Name = S"s10A75";
			this->s10A75->Size = System::Drawing::Size(64, 20);
			this->s10A75->TabIndex = 108;
			this->s10A75->Text = S"XXX.XX °F";
			// 
			// s10A129
			// 
			this->s10A129->Enabled = false;
			this->s10A129->Location = System::Drawing::Point(512, 320);
			this->s10A129->Name = S"s10A129";
			this->s10A129->Size = System::Drawing::Size(64, 20);
			this->s10A129->TabIndex = 111;
			this->s10A129->Text = S"XXX.XX °F";
			// 
			// s10A126
			// 
			this->s10A126->Enabled = false;
			this->s10A126->Location = System::Drawing::Point(512, 296);
			this->s10A126->Name = S"s10A126";
			this->s10A126->Size = System::Drawing::Size(64, 20);
			this->s10A126->TabIndex = 110;
			this->s10A126->Text = S"XXX.XX °F";
			// 
			// s10A78
			// 
			this->s10A78->Enabled = false;
			this->s10A78->Location = System::Drawing::Point(320, 272);
			this->s10A78->Name = S"s10A78";
			this->s10A78->Size = System::Drawing::Size(64, 20);
			this->s10A78->TabIndex = 109;
			this->s10A78->Text = S"XXX.XX °F";
			// 
			// s10A131
			// 
			this->s10A131->Enabled = false;
			this->s10A131->Location = System::Drawing::Point(320, 320);
			this->s10A131->Name = S"s10A131";
			this->s10A131->Size = System::Drawing::Size(64, 20);
			this->s10A131->TabIndex = 114;
			this->s10A131->Text = S"XXX.XX °F";
			// 
			// s10A130
			// 
			this->s10A130->Enabled = false;
			this->s10A130->Location = System::Drawing::Point(320, 296);
			this->s10A130->Name = S"s10A130";
			this->s10A130->Size = System::Drawing::Size(64, 20);
			this->s10A130->TabIndex = 113;
			this->s10A130->Text = S"XXX.XX °F";
			// 
			// s10A132
			// 
			this->s10A132->Enabled = false;
			this->s10A132->Location = System::Drawing::Point(512, 344);
			this->s10A132->Name = S"s10A132";
			this->s10A132->Size = System::Drawing::Size(64, 20);
			this->s10A132->TabIndex = 112;
			this->s10A132->Text = S"XXX.XX °F";
			// 
			// s10A67
			// 
			this->s10A67->Enabled = false;
			this->s10A67->Location = System::Drawing::Point(320, 344);
			this->s10A67->Name = S"s10A67";
			this->s10A67->Size = System::Drawing::Size(64, 20);
			this->s10A67->TabIndex = 115;
			this->s10A67->Text = S"XXX.XX °F";
			// 
			// s11A85
			// 
			this->s11A85->Enabled = false;
			this->s11A85->Location = System::Drawing::Point(320, 416);
			this->s11A85->Name = S"s11A85";
			this->s11A85->Size = System::Drawing::Size(64, 20);
			this->s11A85->TabIndex = 118;
			this->s11A85->Text = S"XXX.XX A";
			// 
			// s11A84
			// 
			this->s11A84->Enabled = false;
			this->s11A84->Location = System::Drawing::Point(320, 392);
			this->s11A84->Name = S"s11A84";
			this->s11A84->Size = System::Drawing::Size(64, 20);
			this->s11A84->TabIndex = 117;
			this->s11A84->Text = S"XXX.XX A";
			// 
			// s11A76
			// 
			this->s11A76->Enabled = false;
			this->s11A76->Location = System::Drawing::Point(320, 368);
			this->s11A76->Name = S"s11A76";
			this->s11A76->Size = System::Drawing::Size(64, 20);
			this->s11A76->TabIndex = 116;
			this->s11A76->Text = S"XXX.XX A";
			// 
			// s11A77
			// 
			this->s11A77->Enabled = false;
			this->s11A77->Location = System::Drawing::Point(320, 152);
			this->s11A77->Name = S"s11A77";
			this->s11A77->Size = System::Drawing::Size(64, 20);
			this->s11A77->TabIndex = 119;
			this->s11A77->Text = S"0.XXX PPH";
			// 
			// s11A78
			// 
			this->s11A78->Enabled = false;
			this->s11A78->Location = System::Drawing::Point(320, 176);
			this->s11A78->Name = S"s11A78";
			this->s11A78->Size = System::Drawing::Size(64, 20);
			this->s11A78->TabIndex = 120;
			this->s11A78->Text = S"0.XXX PPH";
			// 
			// s11A79
			// 
			this->s11A79->Enabled = false;
			this->s11A79->Location = System::Drawing::Point(320, 200);
			this->s11A79->Name = S"s11A79";
			this->s11A79->Size = System::Drawing::Size(64, 20);
			this->s11A79->TabIndex = 121;
			this->s11A79->Text = S"0.XXX PPH";
			// 
			// s11A80
			// 
			this->s11A80->Enabled = false;
			this->s11A80->Location = System::Drawing::Point(512, 152);
			this->s11A80->Name = S"s11A80";
			this->s11A80->Size = System::Drawing::Size(64, 20);
			this->s11A80->TabIndex = 122;
			this->s11A80->Text = S"X.XXX PPH";
			// 
			// s11A81
			// 
			this->s11A81->Enabled = false;
			this->s11A81->Location = System::Drawing::Point(512, 176);
			this->s11A81->Name = S"s11A81";
			this->s11A81->Size = System::Drawing::Size(64, 20);
			this->s11A81->TabIndex = 123;
			this->s11A81->Text = S"X.XXX PPH";
			// 
			// s11A82
			// 
			this->s11A82->Enabled = false;
			this->s11A82->Location = System::Drawing::Point(512, 200);
			this->s11A82->Name = S"s11A82";
			this->s11A82->Size = System::Drawing::Size(64, 20);
			this->s11A82->TabIndex = 124;
			this->s11A82->Text = S"X.XXX PPH";
			// 
			// s11A35
			// 
			this->s11A35->Enabled = false;
			this->s11A35->Location = System::Drawing::Point(512, 56);
			this->s11A35->Name = S"s11A35";
			this->s11A35->Size = System::Drawing::Size(64, 20);
			this->s11A35->TabIndex = 62;
			this->s11A35->Text = S"XXXX PSIA";
			// 
			// s11A30
			// 
			this->s11A30->Enabled = false;
			this->s11A30->Location = System::Drawing::Point(512, 32);
			this->s11A30->Name = S"s11A30";
			this->s11A30->Size = System::Drawing::Size(64, 20);
			this->s11A30->TabIndex = 61;
			this->s11A30->Text = S"XXXX PSIA";
			// 
			// s11A29
			// 
			this->s11A29->Enabled = false;
			this->s11A29->Location = System::Drawing::Point(512, 8);
			this->s11A29->Name = S"s11A29";
			this->s11A29->Size = System::Drawing::Size(64, 20);
			this->s11A29->TabIndex = 60;
			this->s11A29->Text = S"XXXX PSIA";
			// 
			// label29
			// 
			this->label29->Location = System::Drawing::Point(392, 56);
			this->label29->Name = S"label29";
			this->label29->Size = System::Drawing::Size(120, 24);
			this->label29->TabIndex = 52;
			this->label29->Text = S"FC 3 N2 PRESS";
			this->label29->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label28
			// 
			this->label28->Location = System::Drawing::Point(392, 32);
			this->label28->Name = S"label28";
			this->label28->Size = System::Drawing::Size(120, 24);
			this->label28->TabIndex = 51;
			this->label28->Text = S"FC 2 N2 PRESS";
			this->label28->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label27
			// 
			this->label27->Location = System::Drawing::Point(392, 8);
			this->label27->Name = S"label27";
			this->label27->Size = System::Drawing::Size(120, 24);
			this->label27->TabIndex = 50;
			this->label27->Text = S"FC 1 N2 PRESS";
			this->label27->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// EPSForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(600, 518);
			this->Controls->Add(this->s11A82);
			this->Controls->Add(this->s11A81);
			this->Controls->Add(this->s11A80);
			this->Controls->Add(this->s11A79);
			this->Controls->Add(this->s11A78);
			this->Controls->Add(this->s11A77);
			this->Controls->Add(this->s11A85);
			this->Controls->Add(this->s11A84);
			this->Controls->Add(this->s11A76);
			this->Controls->Add(this->s10A67);
			this->Controls->Add(this->s10A131);
			this->Controls->Add(this->s10A130);
			this->Controls->Add(this->s10A132);
			this->Controls->Add(this->s10A129);
			this->Controls->Add(this->s10A126);
			this->Controls->Add(this->s10A78);
			this->Controls->Add(this->s10A75);
			this->Controls->Add(this->s10A72);
			this->Controls->Add(this->s10A69);
			this->Controls->Add(this->s10A123);
			this->Controls->Add(this->s10A51);
			this->Controls->Add(this->s11A47);
			this->Controls->Add(this->s11E283);
			this->Controls->Add(this->s11E282);
			this->Controls->Add(this->s11E281);
			this->Controls->Add(this->label60);
			this->Controls->Add(this->label59);
			this->Controls->Add(this->label58);
			this->Controls->Add(this->label57);
			this->Controls->Add(this->label54);
			this->Controls->Add(this->label55);
			this->Controls->Add(this->label56);
			this->Controls->Add(this->label51);
			this->Controls->Add(this->label52);
			this->Controls->Add(this->label53);
			this->Controls->Add(this->label48);
			this->Controls->Add(this->label49);
			this->Controls->Add(this->label50);
			this->Controls->Add(this->label45);
			this->Controls->Add(this->label46);
			this->Controls->Add(this->label47);
			this->Controls->Add(this->label42);
			this->Controls->Add(this->label43);
			this->Controls->Add(this->label44);
			this->Controls->Add(this->label39);
			this->Controls->Add(this->label40);
			this->Controls->Add(this->label41);
			this->Controls->Add(this->label36);
			this->Controls->Add(this->label37);
			this->Controls->Add(this->label38);
			this->Controls->Add(this->s11A72);
			this->Controls->Add(this->s11A71);
			this->Controls->Add(this->s11A70);
			this->Controls->Add(this->label33);
			this->Controls->Add(this->label34);
			this->Controls->Add(this->label35);
			this->Controls->Add(this->s11A69);
			this->Controls->Add(this->s11A68);
			this->Controls->Add(this->s11A67);
			this->Controls->Add(this->label30);
			this->Controls->Add(this->label31);
			this->Controls->Add(this->label32);
			this->Controls->Add(this->s11A35);
			this->Controls->Add(this->s11A30);
			this->Controls->Add(this->s11A29);
			this->Controls->Add(this->s11A110);
			this->Controls->Add(this->s11A109);
			this->Controls->Add(this->s11A74);
			this->Controls->Add(this->s11A73);
			this->Controls->Add(this->s11A75);
			this->Controls->Add(this->s11A93);
			this->Controls->Add(this->s11A91);
			this->Controls->Add(this->label29);
			this->Controls->Add(this->label28);
			this->Controls->Add(this->label27);
			this->Controls->Add(this->label26);
			this->Controls->Add(this->label25);
			this->Controls->Add(this->label24);
			this->Controls->Add(this->label23);
			this->Controls->Add(this->label22);
			this->Controls->Add(this->label21);
			this->Controls->Add(this->label20);
			this->Controls->Add(this->s11A58);
			this->Controls->Add(this->s11A57);
			this->Controls->Add(this->s11A56);
			this->Controls->Add(this->s11A147);
			this->Controls->Add(this->label19);
			this->Controls->Add(this->label18);
			this->Controls->Add(this->label17);
			this->Controls->Add(this->label16);
			this->Controls->Add(this->s10A89);
			this->Controls->Add(this->s10A88);
			this->Controls->Add(this->s10A86);
			this->Controls->Add(this->label15);
			this->Controls->Add(this->label14);
			this->Controls->Add(this->label13);
			this->Controls->Add(this->s10A63);
			this->Controls->Add(this->s10A60);
			this->Controls->Add(this->s10A57);
			this->Controls->Add(this->s10A54);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->s10A39);
			this->Controls->Add(this->s10A36);
			this->Controls->Add(this->s10A66);
			this->Controls->Add(this->s10A150);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->s10A42);
			this->Controls->Add(this->s10A147);
			this->Controls->Add(this->s10A144);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->s10A141);
			this->Controls->Add(this->label1);
			this->Name = S"EPSForm";
			this->Text = S"ELECTRICAL POWER SYSTEM";
			this->ResumeLayout(false);

		}		


};
	
}