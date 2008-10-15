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
	/// Summary for StructuresForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	__gc class StructuresForm : public System::Windows::Forms::Form
	{
	public: 
		StructuresForm(void)
		{
			InitializeComponent();
			this->add_Closing(new CancelEventHandler(this, &GroundStation::StructuresForm::StructuresForm_Closing));
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
	private: System::Void StructuresForm_Closing(Object * sender, System::ComponentModel::CancelEventArgs* e){			
			e->Cancel = true; // don't close			
			this->Hide();     // Hide instead.
		}      		
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::TextBox *  s10A65;
	private: System::Windows::Forms::TextBox *  s10A70;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::TextBox *  s10A74;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::TextBox *  s10A79;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::TextBox *  s10A142;
	private: System::Windows::Forms::Label *  label6;
	private: System::Windows::Forms::Label *  label7;
	private: System::Windows::Forms::Label *  label8;
	private: System::Windows::Forms::TextBox *  s10A137;
	private: System::Windows::Forms::TextBox *  s10A139;
	private: System::Windows::Forms::TextBox *  s10A140;
	private: System::Windows::Forms::Label *  label9;
	private: System::Windows::Forms::Label *  label10;
	private: System::Windows::Forms::TextBox *  s10A143;
	private: System::Windows::Forms::TextBox *  s10A145;



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
			this->s10A65 = new System::Windows::Forms::TextBox();
			this->s10A70 = new System::Windows::Forms::TextBox();
			this->label3 = new System::Windows::Forms::Label();
			this->s10A74 = new System::Windows::Forms::TextBox();
			this->label4 = new System::Windows::Forms::Label();
			this->s10A79 = new System::Windows::Forms::TextBox();
			this->label5 = new System::Windows::Forms::Label();
			this->s10A142 = new System::Windows::Forms::TextBox();
			this->label6 = new System::Windows::Forms::Label();
			this->label7 = new System::Windows::Forms::Label();
			this->label8 = new System::Windows::Forms::Label();
			this->s10A137 = new System::Windows::Forms::TextBox();
			this->s10A139 = new System::Windows::Forms::TextBox();
			this->s10A140 = new System::Windows::Forms::TextBox();
			this->label9 = new System::Windows::Forms::Label();
			this->label10 = new System::Windows::Forms::Label();
			this->s10A143 = new System::Windows::Forms::TextBox();
			this->s10A145 = new System::Windows::Forms::TextBox();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 8);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(160, 24);
			this->label1->TabIndex = 0;
			this->label1->Text = S"SIDE HS BOND LOC 1 TEMP";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 32);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(160, 24);
			this->label2->TabIndex = 1;
			this->label2->Text = S"SIDE HS BOND LOC 4 TEMP";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A65
			// 
			this->s10A65->Enabled = false;
			this->s10A65->Location = System::Drawing::Point(176, 8);
			this->s10A65->Name = S"s10A65";
			this->s10A65->Size = System::Drawing::Size(64, 20);
			this->s10A65->TabIndex = 2;
			this->s10A65->Text = S"XXX.XX °F";
			// 
			// s10A70
			// 
			this->s10A70->Enabled = false;
			this->s10A70->Location = System::Drawing::Point(176, 32);
			this->s10A70->Name = S"s10A70";
			this->s10A70->Size = System::Drawing::Size(64, 20);
			this->s10A70->TabIndex = 3;
			this->s10A70->Text = S"XXX.XX °F";
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 56);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(160, 24);
			this->label3->TabIndex = 4;
			this->label3->Text = S"SIDE HS BOND LOC 7 TEMP";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A74
			// 
			this->s10A74->Enabled = false;
			this->s10A74->Location = System::Drawing::Point(176, 56);
			this->s10A74->Name = S"s10A74";
			this->s10A74->Size = System::Drawing::Size(64, 20);
			this->s10A74->TabIndex = 5;
			this->s10A74->Text = S"XXX.XX °F";
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(8, 80);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(168, 24);
			this->label4->TabIndex = 6;
			this->label4->Text = S"SIDE HS BOND LOC 10 TEMP";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A79
			// 
			this->s10A79->Enabled = false;
			this->s10A79->Location = System::Drawing::Point(176, 80);
			this->s10A79->Name = S"s10A79";
			this->s10A79->Size = System::Drawing::Size(64, 20);
			this->s10A79->TabIndex = 7;
			this->s10A79->Text = S"XXX.XX °F";
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(8, 104);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(168, 24);
			this->label5->TabIndex = 8;
			this->label5->Text = S"BAY 2 OXY TK SURF TEMP";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A142
			// 
			this->s10A142->Enabled = false;
			this->s10A142->Location = System::Drawing::Point(176, 104);
			this->s10A142->Name = S"s10A142";
			this->s10A142->Size = System::Drawing::Size(64, 20);
			this->s10A142->TabIndex = 9;
			this->s10A142->Text = S"XXX.XX °F";
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(8, 128);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(168, 24);
			this->label6->TabIndex = 10;
			this->label6->Text = S"BAY 3 OXY TK SURF TEMP";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(8, 152);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(168, 24);
			this->label7->TabIndex = 11;
			this->label7->Text = S"BAY 5 FUEL TK SURF TEMP";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(8, 176);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(168, 24);
			this->label8->TabIndex = 12;
			this->label8->Text = S"BAY 6 FUEL TK SURF TEMP";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A137
			// 
			this->s10A137->Enabled = false;
			this->s10A137->Location = System::Drawing::Point(176, 128);
			this->s10A137->Name = S"s10A137";
			this->s10A137->Size = System::Drawing::Size(64, 20);
			this->s10A137->TabIndex = 13;
			this->s10A137->Text = S"XXX.XX °F";
			// 
			// s10A139
			// 
			this->s10A139->Enabled = false;
			this->s10A139->Location = System::Drawing::Point(176, 152);
			this->s10A139->Name = S"s10A139";
			this->s10A139->Size = System::Drawing::Size(64, 20);
			this->s10A139->TabIndex = 14;
			this->s10A139->Text = S"XXX.XX °F";
			// 
			// s10A140
			// 
			this->s10A140->Enabled = false;
			this->s10A140->Location = System::Drawing::Point(176, 176);
			this->s10A140->Name = S"s10A140";
			this->s10A140->Size = System::Drawing::Size(64, 20);
			this->s10A140->TabIndex = 15;
			this->s10A140->Text = S"XXX.XX °F";
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(8, 200);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(168, 24);
			this->label9->TabIndex = 16;
			this->label9->Text = S"OX LN ENTRY SUMP TK TEMP";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 224);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(168, 24);
			this->label10->TabIndex = 17;
			this->label10->Text = S"FU LN ENTRY SUMP TK TEMP";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// s10A143
			// 
			this->s10A143->Enabled = false;
			this->s10A143->Location = System::Drawing::Point(176, 200);
			this->s10A143->Name = S"s10A143";
			this->s10A143->Size = System::Drawing::Size(64, 20);
			this->s10A143->TabIndex = 18;
			this->s10A143->Text = S"XXX.XX °F";
			// 
			// s10A145
			// 
			this->s10A145->Enabled = false;
			this->s10A145->Location = System::Drawing::Point(176, 224);
			this->s10A145->Name = S"s10A145";
			this->s10A145->Size = System::Drawing::Size(64, 20);
			this->s10A145->TabIndex = 19;
			this->s10A145->Text = S"XXX.XX °F";
			// 
			// StructuresForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(246, 252);
			this->Controls->Add(this->s10A145);
			this->Controls->Add(this->s10A143);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->s10A140);
			this->Controls->Add(this->s10A139);
			this->Controls->Add(this->s10A137);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->s10A142);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->s10A79);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->s10A74);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->s10A70);
			this->Controls->Add(this->s10A65);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = S"StructuresForm";
			this->Text = S"STRUCTURES ";
			this->Load += new System::EventHandler(this, &GroundStation::StructuresForm::StructuresForm_Load);
			this->ResumeLayout(false);

		}		

private: System::Void StructuresForm_Load(System::Object *  sender, System::EventArgs *  e)
		 {
		 }

};
}