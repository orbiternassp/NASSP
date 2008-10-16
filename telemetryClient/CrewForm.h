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
	/// Summary for CrewForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	__gc class CrewForm : public System::Windows::Forms::Form
	{
	public: 
		CrewForm(void)
		{
			InitializeComponent();
			this->add_Closing(new CancelEventHandler(this, &GroundStation::CrewForm::CrewForm_Closing));
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
	private: System::Void CrewForm_Closing(Object * sender, System::ComponentModel::CancelEventArgs* e){			
			e->Cancel = true; // don't close			
			this->Hide();     // Hide instead.
		}      		
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::Label *  label6;
	public: System::Windows::Forms::TextBox *  s22A3;
	public: System::Windows::Forms::TextBox *  s22A1;
	public: System::Windows::Forms::TextBox *  s22A2;
	public: System::Windows::Forms::TextBox *  s51A7;
	public: System::Windows::Forms::TextBox *  s51A8;
	public: System::Windows::Forms::TextBox *  s51A9;
	private: System::Windows::Forms::GroupBox *  groupBox1;
	private: System::Windows::Forms::Label *  label23;
	private: System::Windows::Forms::Label *  label24;
	private: System::Windows::Forms::Label *  label25;
	private: System::Windows::Forms::Label *  label26;
	private: System::Windows::Forms::Label *  label11;
	private: System::Windows::Forms::Label *  label12;
	private: System::Windows::Forms::Label *  label13;
	private: System::Windows::Forms::Label *  label14;
	private: System::Windows::Forms::Label *  label10;
	private: System::Windows::Forms::Label *  label9;
	private: System::Windows::Forms::Label *  label8;
	private: System::Windows::Forms::Label *  label7;
	private: System::Windows::Forms::Label *  label19;
	private: System::Windows::Forms::Label *  label20;
	private: System::Windows::Forms::Label *  label21;
	private: System::Windows::Forms::Label *  label22;
	private: System::Windows::Forms::Label *  label15;
	private: System::Windows::Forms::Label *  label16;
	private: System::Windows::Forms::Label *  label17;
	private: System::Windows::Forms::Label *  label18;
	private: System::Windows::Forms::Label *  label27;
	private: System::Windows::Forms::Label *  label28;
	private: System::Windows::Forms::Label *  label29;
	private: System::Windows::Forms::Label *  label30;
	private: System::Windows::Forms::Label *  label31;
	private: System::Windows::Forms::GroupBox *  groupBox2;
	public: System::Windows::Forms::TextBox *  s10A108;
	public: System::Windows::Forms::TextBox *  s10A111;
	public: System::Windows::Forms::TextBox *  s10A104;
	public: System::Windows::Forms::TextBox *  s10A106;

	public: System::Windows::Forms::TextBox *  s10A110;
	public: System::Windows::Forms::TextBox *  s10A109;
	public: System::Windows::Forms::TextBox *  s10A107;
	public: System::Windows::Forms::TextBox *  s11D18;
	public: System::Windows::Forms::TextBox *  s11D17;
	public: System::Windows::Forms::TextBox *  s11D12;
	public: System::Windows::Forms::TextBox *  s11D11;
	public: System::Windows::Forms::TextBox *  s11D10;
	public: System::Windows::Forms::TextBox *  s11D7;
	public: System::Windows::Forms::TextBox *  s11D6;
	public: System::Windows::Forms::TextBox *  s11D5;
	public: System::Windows::Forms::TextBox *  s11D3;
	public: System::Windows::Forms::TextBox *  s11D16;
	public: System::Windows::Forms::TextBox *  s10A122;
	public: System::Windows::Forms::TextBox *  s10A121;
	public: System::Windows::Forms::TextBox *  s10A119;
	public: System::Windows::Forms::TextBox *  s10A118;
	public: System::Windows::Forms::TextBox *  s10A116;
	public: System::Windows::Forms::TextBox *  s10A115;
	public: System::Windows::Forms::TextBox *  s10A113;
	public: System::Windows::Forms::TextBox *  s10A112;
























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
			this->s22A3 = new System::Windows::Forms::TextBox();
			this->s22A1 = new System::Windows::Forms::TextBox();
			this->s22A2 = new System::Windows::Forms::TextBox();
			this->s51A7 = new System::Windows::Forms::TextBox();
			this->s51A8 = new System::Windows::Forms::TextBox();
			this->s51A9 = new System::Windows::Forms::TextBox();
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->label23 = new System::Windows::Forms::Label();
			this->label24 = new System::Windows::Forms::Label();
			this->label25 = new System::Windows::Forms::Label();
			this->label26 = new System::Windows::Forms::Label();
			this->label11 = new System::Windows::Forms::Label();
			this->label12 = new System::Windows::Forms::Label();
			this->label13 = new System::Windows::Forms::Label();
			this->label14 = new System::Windows::Forms::Label();
			this->label10 = new System::Windows::Forms::Label();
			this->label9 = new System::Windows::Forms::Label();
			this->label8 = new System::Windows::Forms::Label();
			this->label7 = new System::Windows::Forms::Label();
			this->label19 = new System::Windows::Forms::Label();
			this->label20 = new System::Windows::Forms::Label();
			this->label21 = new System::Windows::Forms::Label();
			this->label22 = new System::Windows::Forms::Label();
			this->label15 = new System::Windows::Forms::Label();
			this->label16 = new System::Windows::Forms::Label();
			this->label17 = new System::Windows::Forms::Label();
			this->label18 = new System::Windows::Forms::Label();
			this->label27 = new System::Windows::Forms::Label();
			this->label28 = new System::Windows::Forms::Label();
			this->label29 = new System::Windows::Forms::Label();
			this->label30 = new System::Windows::Forms::Label();
			this->label31 = new System::Windows::Forms::Label();
			this->groupBox2 = new System::Windows::Forms::GroupBox();
			this->s11D18 = new System::Windows::Forms::TextBox();
			this->s11D17 = new System::Windows::Forms::TextBox();
			this->s11D12 = new System::Windows::Forms::TextBox();
			this->s11D11 = new System::Windows::Forms::TextBox();
			this->s11D10 = new System::Windows::Forms::TextBox();
			this->s11D7 = new System::Windows::Forms::TextBox();
			this->s11D6 = new System::Windows::Forms::TextBox();
			this->s11D5 = new System::Windows::Forms::TextBox();
			this->s11D3 = new System::Windows::Forms::TextBox();
			this->s11D16 = new System::Windows::Forms::TextBox();
			this->s10A122 = new System::Windows::Forms::TextBox();
			this->s10A121 = new System::Windows::Forms::TextBox();
			this->s10A119 = new System::Windows::Forms::TextBox();
			this->s10A118 = new System::Windows::Forms::TextBox();
			this->s10A116 = new System::Windows::Forms::TextBox();
			this->s10A115 = new System::Windows::Forms::TextBox();
			this->s10A113 = new System::Windows::Forms::TextBox();
			this->s10A112 = new System::Windows::Forms::TextBox();
			this->s10A110 = new System::Windows::Forms::TextBox();
			this->s10A109 = new System::Windows::Forms::TextBox();
			this->s10A107 = new System::Windows::Forms::TextBox();
			this->s10A106 = new System::Windows::Forms::TextBox();
			this->s10A104 = new System::Windows::Forms::TextBox();
			this->s10A111 = new System::Windows::Forms::TextBox();
			this->s10A108 = new System::Windows::Forms::TextBox();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 16);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(120, 24);
			this->label1->TabIndex = 4;
			this->label1->Text = S"ASTRO 1 EKG AXIS 1";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 40);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(120, 24);
			this->label2->TabIndex = 5;
			this->label2->Text = S"ASTRO 2 EKG AXIS 1";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 64);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(120, 24);
			this->label3->TabIndex = 6;
			this->label3->Text = S"ASTRO 3 EKG AXIS 1";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(8, 88);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(120, 24);
			this->label4->TabIndex = 7;
			this->label4->Text = S"ASTRO 1 RESPIR";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(8, 112);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(120, 24);
			this->label5->TabIndex = 8;
			this->label5->Text = S"ASTRO 2 RESPIR";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(8, 136);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(120, 24);
			this->label6->TabIndex = 9;
			this->label6->Text = S"ASTRO 3 RESPIR";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s22A3
			// 
			this->s22A3->Enabled = false;
			this->s22A3->Location = System::Drawing::Point(128, 16);
			this->s22A3->Name = S"s22A3";
			this->s22A3->Size = System::Drawing::Size(64, 20);
			this->s22A3->TabIndex = 101;
			this->s22A3->Text = S"X.XXXX MV";
			// 
			// s22A1
			// 
			this->s22A1->Enabled = false;
			this->s22A1->Location = System::Drawing::Point(128, 40);
			this->s22A1->Name = S"s22A1";
			this->s22A1->Size = System::Drawing::Size(64, 20);
			this->s22A1->TabIndex = 102;
			this->s22A1->Text = S"X.XXXX MV";
			// 
			// s22A2
			// 
			this->s22A2->Enabled = false;
			this->s22A2->Location = System::Drawing::Point(128, 64);
			this->s22A2->Name = S"s22A2";
			this->s22A2->Size = System::Drawing::Size(64, 20);
			this->s22A2->TabIndex = 103;
			this->s22A2->Text = S"X.XXXX MV";
			// 
			// s51A7
			// 
			this->s51A7->Enabled = false;
			this->s51A7->Location = System::Drawing::Point(128, 88);
			this->s51A7->Name = S"s51A7";
			this->s51A7->Size = System::Drawing::Size(64, 20);
			this->s51A7->TabIndex = 104;
			this->s51A7->Text = S"X.XXXX O";
			// 
			// s51A8
			// 
			this->s51A8->Enabled = false;
			this->s51A8->Location = System::Drawing::Point(128, 112);
			this->s51A8->Name = S"s51A8";
			this->s51A8->Size = System::Drawing::Size(64, 20);
			this->s51A8->TabIndex = 105;
			this->s51A8->Text = S"X.XXXX O";
			// 
			// s51A9
			// 
			this->s51A9->Enabled = false;
			this->s51A9->Location = System::Drawing::Point(128, 136);
			this->s51A9->Name = S"s51A9";
			this->s51A9->Size = System::Drawing::Size(64, 20);
			this->s51A9->TabIndex = 106;
			this->s51A9->Text = S"X.XXXX O";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->s22A3);
			this->groupBox1->Controls->Add(this->s22A1);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->s22A2);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->s51A7);
			this->groupBox1->Controls->Add(this->s51A8);
			this->groupBox1->Controls->Add(this->s51A9);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Location = System::Drawing::Point(8, 8);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(200, 168);
			this->groupBox1->TabIndex = 107;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"CREW LIFE SYSTEM";
			// 
			// label23
			// 
			this->label23->Location = System::Drawing::Point(176, 112);
			this->label23->Name = S"label23";
			this->label23->Size = System::Drawing::Size(96, 24);
			this->label23->TabIndex = 127;
			this->label23->Text = S"SCI EXP NO. 20";
			this->label23->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label24
			// 
			this->label24->Location = System::Drawing::Point(176, 88);
			this->label24->Name = S"label24";
			this->label24->Size = System::Drawing::Size(96, 24);
			this->label24->TabIndex = 126;
			this->label24->Text = S"SCI EXP NO. 19";
			this->label24->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label25
			// 
			this->label25->Location = System::Drawing::Point(176, 64);
			this->label25->Name = S"label25";
			this->label25->Size = System::Drawing::Size(96, 24);
			this->label25->TabIndex = 125;
			this->label25->Text = S"SCI EXP NO. 18";
			this->label25->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label26
			// 
			this->label26->Location = System::Drawing::Point(176, 40);
			this->label26->Name = S"label26";
			this->label26->Size = System::Drawing::Size(96, 24);
			this->label26->TabIndex = 124;
			this->label26->Text = S"SCI EXP NO. 17";
			this->label26->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(8, 184);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(96, 24);
			this->label11->TabIndex = 135;
			this->label11->Text = S"SCI EXP NO. 8";
			this->label11->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label12
			// 
			this->label12->Location = System::Drawing::Point(8, 160);
			this->label12->Name = S"label12";
			this->label12->Size = System::Drawing::Size(96, 24);
			this->label12->TabIndex = 134;
			this->label12->Text = S"SCI EXP NO. 7";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label13
			// 
			this->label13->Location = System::Drawing::Point(8, 136);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(96, 24);
			this->label13->TabIndex = 133;
			this->label13->Text = S"SCI EXP NO. 6";
			this->label13->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label14
			// 
			this->label14->Location = System::Drawing::Point(8, 112);
			this->label14->Name = S"label14";
			this->label14->Size = System::Drawing::Size(96, 24);
			this->label14->TabIndex = 132;
			this->label14->Text = S"SCI EXP NO. 5";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 88);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(96, 24);
			this->label10->TabIndex = 131;
			this->label10->Text = S"SCI EXP NO. 4";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(8, 64);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(96, 24);
			this->label9->TabIndex = 130;
			this->label9->Text = S"SCI EXP NO. 3";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(8, 40);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(96, 24);
			this->label8->TabIndex = 129;
			this->label8->Text = S"SCI EXP NO. 2";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(8, 16);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(96, 24);
			this->label7->TabIndex = 128;
			this->label7->Text = S"SCI EXP NO. 1";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label19
			// 
			this->label19->Location = System::Drawing::Point(176, 16);
			this->label19->Name = S"label19";
			this->label19->Size = System::Drawing::Size(96, 24);
			this->label19->TabIndex = 143;
			this->label19->Text = S"SCI EXP NO. 16";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label20
			// 
			this->label20->Location = System::Drawing::Point(8, 352);
			this->label20->Name = S"label20";
			this->label20->Size = System::Drawing::Size(96, 24);
			this->label20->TabIndex = 142;
			this->label20->Text = S"SCI EXP NO. 15";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label21
			// 
			this->label21->Location = System::Drawing::Point(8, 328);
			this->label21->Name = S"label21";
			this->label21->Size = System::Drawing::Size(96, 24);
			this->label21->TabIndex = 141;
			this->label21->Text = S"SCI EXP NO. 14";
			this->label21->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label22
			// 
			this->label22->Location = System::Drawing::Point(8, 304);
			this->label22->Name = S"label22";
			this->label22->Size = System::Drawing::Size(96, 24);
			this->label22->TabIndex = 140;
			this->label22->Text = S"SCI EXP NO. 13";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label15
			// 
			this->label15->Location = System::Drawing::Point(8, 280);
			this->label15->Name = S"label15";
			this->label15->Size = System::Drawing::Size(96, 24);
			this->label15->TabIndex = 139;
			this->label15->Text = S"SCI EXP NO. 12";
			this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label16
			// 
			this->label16->Location = System::Drawing::Point(8, 256);
			this->label16->Name = S"label16";
			this->label16->Size = System::Drawing::Size(96, 24);
			this->label16->TabIndex = 138;
			this->label16->Text = S"SCI EXP NO. 11";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label17
			// 
			this->label17->Location = System::Drawing::Point(8, 232);
			this->label17->Name = S"label17";
			this->label17->Size = System::Drawing::Size(96, 24);
			this->label17->TabIndex = 137;
			this->label17->Text = S"SCI EXP NO. 10";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label18
			// 
			this->label18->Location = System::Drawing::Point(8, 208);
			this->label18->Name = S"label18";
			this->label18->Size = System::Drawing::Size(96, 24);
			this->label18->TabIndex = 136;
			this->label18->Text = S"SCI EXP NO. 9";
			this->label18->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label27
			// 
			this->label27->Location = System::Drawing::Point(176, 208);
			this->label27->Name = S"label27";
			this->label27->Size = System::Drawing::Size(96, 24);
			this->label27->TabIndex = 147;
			this->label27->Text = S"SCI EXP NO. 24";
			this->label27->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label28
			// 
			this->label28->Location = System::Drawing::Point(176, 184);
			this->label28->Name = S"label28";
			this->label28->Size = System::Drawing::Size(96, 24);
			this->label28->TabIndex = 146;
			this->label28->Text = S"SCI EXP NO. 23";
			this->label28->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label29
			// 
			this->label29->Location = System::Drawing::Point(176, 160);
			this->label29->Name = S"label29";
			this->label29->Size = System::Drawing::Size(96, 24);
			this->label29->TabIndex = 145;
			this->label29->Text = S"SCI EXP NO. 22";
			this->label29->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label30
			// 
			this->label30->Location = System::Drawing::Point(176, 136);
			this->label30->Name = S"label30";
			this->label30->Size = System::Drawing::Size(96, 24);
			this->label30->TabIndex = 144;
			this->label30->Text = S"SCI EXP NO. 21";
			this->label30->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label31
			// 
			this->label31->Location = System::Drawing::Point(176, 232);
			this->label31->Name = S"label31";
			this->label31->Size = System::Drawing::Size(96, 24);
			this->label31->TabIndex = 148;
			this->label31->Text = S"SCI EXP NO. 25";
			this->label31->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->s11D18);
			this->groupBox2->Controls->Add(this->s11D17);
			this->groupBox2->Controls->Add(this->s11D12);
			this->groupBox2->Controls->Add(this->s11D11);
			this->groupBox2->Controls->Add(this->s11D10);
			this->groupBox2->Controls->Add(this->s11D7);
			this->groupBox2->Controls->Add(this->s11D6);
			this->groupBox2->Controls->Add(this->s11D5);
			this->groupBox2->Controls->Add(this->s11D3);
			this->groupBox2->Controls->Add(this->s11D16);
			this->groupBox2->Controls->Add(this->s10A122);
			this->groupBox2->Controls->Add(this->s10A121);
			this->groupBox2->Controls->Add(this->s10A119);
			this->groupBox2->Controls->Add(this->s10A118);
			this->groupBox2->Controls->Add(this->s10A116);
			this->groupBox2->Controls->Add(this->s10A115);
			this->groupBox2->Controls->Add(this->s10A113);
			this->groupBox2->Controls->Add(this->s10A112);
			this->groupBox2->Controls->Add(this->s10A110);
			this->groupBox2->Controls->Add(this->s10A109);
			this->groupBox2->Controls->Add(this->s10A107);
			this->groupBox2->Controls->Add(this->s10A106);
			this->groupBox2->Controls->Add(this->s10A104);
			this->groupBox2->Controls->Add(this->s10A111);
			this->groupBox2->Controls->Add(this->s10A108);
			this->groupBox2->Controls->Add(this->label13);
			this->groupBox2->Controls->Add(this->label14);
			this->groupBox2->Controls->Add(this->label10);
			this->groupBox2->Controls->Add(this->label9);
			this->groupBox2->Controls->Add(this->label8);
			this->groupBox2->Controls->Add(this->label20);
			this->groupBox2->Controls->Add(this->label21);
			this->groupBox2->Controls->Add(this->label22);
			this->groupBox2->Controls->Add(this->label15);
			this->groupBox2->Controls->Add(this->label16);
			this->groupBox2->Controls->Add(this->label17);
			this->groupBox2->Controls->Add(this->label18);
			this->groupBox2->Controls->Add(this->label11);
			this->groupBox2->Controls->Add(this->label12);
			this->groupBox2->Controls->Add(this->label7);
			this->groupBox2->Controls->Add(this->label24);
			this->groupBox2->Controls->Add(this->label23);
			this->groupBox2->Controls->Add(this->label19);
			this->groupBox2->Controls->Add(this->label27);
			this->groupBox2->Controls->Add(this->label28);
			this->groupBox2->Controls->Add(this->label29);
			this->groupBox2->Controls->Add(this->label30);
			this->groupBox2->Controls->Add(this->label31);
			this->groupBox2->Controls->Add(this->label26);
			this->groupBox2->Controls->Add(this->label25);
			this->groupBox2->Location = System::Drawing::Point(216, 8);
			this->groupBox2->Name = S"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(344, 384);
			this->groupBox2->TabIndex = 149;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = S"SCIENCE PAYLOAD";
			// 
			// s11D18
			// 
			this->s11D18->Enabled = false;
			this->s11D18->Location = System::Drawing::Point(272, 232);
			this->s11D18->Name = S"s11D18";
			this->s11D18->Size = System::Drawing::Size(64, 20);
			this->s11D18->TabIndex = 173;
			this->s11D18->Text = S"XXXXX";
			// 
			// s11D17
			// 
			this->s11D17->Enabled = false;
			this->s11D17->Location = System::Drawing::Point(272, 208);
			this->s11D17->Name = S"s11D17";
			this->s11D17->Size = System::Drawing::Size(64, 20);
			this->s11D17->TabIndex = 172;
			this->s11D17->Text = S"XXXXX";
			// 
			// s11D12
			// 
			this->s11D12->Enabled = false;
			this->s11D12->Location = System::Drawing::Point(272, 184);
			this->s11D12->Name = S"s11D12";
			this->s11D12->Size = System::Drawing::Size(64, 20);
			this->s11D12->TabIndex = 171;
			this->s11D12->Text = S"XXXXX";
			// 
			// s11D11
			// 
			this->s11D11->Enabled = false;
			this->s11D11->Location = System::Drawing::Point(272, 160);
			this->s11D11->Name = S"s11D11";
			this->s11D11->Size = System::Drawing::Size(64, 20);
			this->s11D11->TabIndex = 170;
			this->s11D11->Text = S"XXXXX";
			// 
			// s11D10
			// 
			this->s11D10->Enabled = false;
			this->s11D10->Location = System::Drawing::Point(272, 136);
			this->s11D10->Name = S"s11D10";
			this->s11D10->Size = System::Drawing::Size(64, 20);
			this->s11D10->TabIndex = 169;
			this->s11D10->Text = S"XXXXX";
			// 
			// s11D7
			// 
			this->s11D7->Enabled = false;
			this->s11D7->Location = System::Drawing::Point(272, 112);
			this->s11D7->Name = S"s11D7";
			this->s11D7->Size = System::Drawing::Size(64, 20);
			this->s11D7->TabIndex = 168;
			this->s11D7->Text = S"XXXXX";
			// 
			// s11D6
			// 
			this->s11D6->Enabled = false;
			this->s11D6->Location = System::Drawing::Point(272, 88);
			this->s11D6->Name = S"s11D6";
			this->s11D6->Size = System::Drawing::Size(64, 20);
			this->s11D6->TabIndex = 167;
			this->s11D6->Text = S"XXXXX";
			// 
			// s11D5
			// 
			this->s11D5->Enabled = false;
			this->s11D5->Location = System::Drawing::Point(272, 64);
			this->s11D5->Name = S"s11D5";
			this->s11D5->Size = System::Drawing::Size(64, 20);
			this->s11D5->TabIndex = 166;
			this->s11D5->Text = S"XXXXX";
			// 
			// s11D3
			// 
			this->s11D3->Enabled = false;
			this->s11D3->Location = System::Drawing::Point(272, 40);
			this->s11D3->Name = S"s11D3";
			this->s11D3->Size = System::Drawing::Size(64, 20);
			this->s11D3->TabIndex = 165;
			this->s11D3->Text = S"XXXXX";
			// 
			// s11D16
			// 
			this->s11D16->Enabled = false;
			this->s11D16->Location = System::Drawing::Point(272, 16);
			this->s11D16->Name = S"s11D16";
			this->s11D16->Size = System::Drawing::Size(64, 20);
			this->s11D16->TabIndex = 164;
			this->s11D16->Text = S"XXXXX";
			// 
			// s10A122
			// 
			this->s10A122->Enabled = false;
			this->s10A122->Location = System::Drawing::Point(104, 352);
			this->s10A122->Name = S"s10A122";
			this->s10A122->Size = System::Drawing::Size(64, 20);
			this->s10A122->TabIndex = 163;
			this->s10A122->Text = S"XXXXX";
			// 
			// s10A121
			// 
			this->s10A121->Enabled = false;
			this->s10A121->Location = System::Drawing::Point(104, 328);
			this->s10A121->Name = S"s10A121";
			this->s10A121->Size = System::Drawing::Size(64, 20);
			this->s10A121->TabIndex = 162;
			this->s10A121->Text = S"XXXXX";
			// 
			// s10A119
			// 
			this->s10A119->Enabled = false;
			this->s10A119->Location = System::Drawing::Point(104, 304);
			this->s10A119->Name = S"s10A119";
			this->s10A119->Size = System::Drawing::Size(64, 20);
			this->s10A119->TabIndex = 161;
			this->s10A119->Text = S"XXXXX";
			// 
			// s10A118
			// 
			this->s10A118->Enabled = false;
			this->s10A118->Location = System::Drawing::Point(104, 280);
			this->s10A118->Name = S"s10A118";
			this->s10A118->Size = System::Drawing::Size(64, 20);
			this->s10A118->TabIndex = 160;
			this->s10A118->Text = S"XXXXX";
			// 
			// s10A116
			// 
			this->s10A116->Enabled = false;
			this->s10A116->Location = System::Drawing::Point(104, 256);
			this->s10A116->Name = S"s10A116";
			this->s10A116->Size = System::Drawing::Size(64, 20);
			this->s10A116->TabIndex = 159;
			this->s10A116->Text = S"XXXXX";
			// 
			// s10A115
			// 
			this->s10A115->Enabled = false;
			this->s10A115->Location = System::Drawing::Point(104, 232);
			this->s10A115->Name = S"s10A115";
			this->s10A115->Size = System::Drawing::Size(64, 20);
			this->s10A115->TabIndex = 158;
			this->s10A115->Text = S"XXXXX";
			// 
			// s10A113
			// 
			this->s10A113->Enabled = false;
			this->s10A113->Location = System::Drawing::Point(104, 208);
			this->s10A113->Name = S"s10A113";
			this->s10A113->Size = System::Drawing::Size(64, 20);
			this->s10A113->TabIndex = 157;
			this->s10A113->Text = S"XXXXX";
			// 
			// s10A112
			// 
			this->s10A112->Enabled = false;
			this->s10A112->Location = System::Drawing::Point(104, 184);
			this->s10A112->Name = S"s10A112";
			this->s10A112->Size = System::Drawing::Size(64, 20);
			this->s10A112->TabIndex = 156;
			this->s10A112->Text = S"XXXXX";
			// 
			// s10A110
			// 
			this->s10A110->Enabled = false;
			this->s10A110->Location = System::Drawing::Point(104, 160);
			this->s10A110->Name = S"s10A110";
			this->s10A110->Size = System::Drawing::Size(64, 20);
			this->s10A110->TabIndex = 155;
			this->s10A110->Text = S"XXXXX";
			// 
			// s10A109
			// 
			this->s10A109->Enabled = false;
			this->s10A109->Location = System::Drawing::Point(104, 136);
			this->s10A109->Name = S"s10A109";
			this->s10A109->Size = System::Drawing::Size(64, 20);
			this->s10A109->TabIndex = 154;
			this->s10A109->Text = S"XXXXX";
			// 
			// s10A107
			// 
			this->s10A107->Enabled = false;
			this->s10A107->Location = System::Drawing::Point(104, 112);
			this->s10A107->Name = S"s10A107";
			this->s10A107->Size = System::Drawing::Size(64, 20);
			this->s10A107->TabIndex = 153;
			this->s10A107->Text = S"XXXXX";
			// 
			// s10A106
			// 
			this->s10A106->Enabled = false;
			this->s10A106->Location = System::Drawing::Point(104, 88);
			this->s10A106->Name = S"s10A106";
			this->s10A106->Size = System::Drawing::Size(64, 20);
			this->s10A106->TabIndex = 152;
			this->s10A106->Text = S"XXXXX";
			// 
			// s10A104
			// 
			this->s10A104->Enabled = false;
			this->s10A104->Location = System::Drawing::Point(104, 64);
			this->s10A104->Name = S"s10A104";
			this->s10A104->Size = System::Drawing::Size(64, 20);
			this->s10A104->TabIndex = 151;
			this->s10A104->Text = S"XXXXX";
			// 
			// s10A111
			// 
			this->s10A111->Enabled = false;
			this->s10A111->Location = System::Drawing::Point(104, 40);
			this->s10A111->Name = S"s10A111";
			this->s10A111->Size = System::Drawing::Size(64, 20);
			this->s10A111->TabIndex = 150;
			this->s10A111->Text = S"XXXXX";
			// 
			// s10A108
			// 
			this->s10A108->Enabled = false;
			this->s10A108->Location = System::Drawing::Point(104, 16);
			this->s10A108->Name = S"s10A108";
			this->s10A108->Size = System::Drawing::Size(64, 20);
			this->s10A108->TabIndex = 149;
			this->s10A108->Text = S"XXXXX";
			// 
			// CrewForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(566, 396);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = S"CrewForm";
			this->Text = S"CREW LIFE SYSTEM / SCIENCE PAYLOAD";
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);

		}		

};
}