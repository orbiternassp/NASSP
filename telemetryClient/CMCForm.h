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
	/// Summary for CMCForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	__gc class CMCForm : public System::Windows::Forms::Form
	{
	public: 
		CMCForm(void)
		{
			InitializeComponent();
			this->add_Closing(new CancelEventHandler(this, &GroundStation::CMCForm::CMCForm_Closing));
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

	private: System::Void CMCForm_Closing(Object * sender, System::ComponentModel::CancelEventArgs* e){			
			e->Cancel = true; // don't close			
			this->Hide();     // Hide instead.
		}      		

	private: System::Windows::Forms::Label *  label1;
	public: System::Windows::Forms::TextBox *  cmcListID;


	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::Label *  label6;
	private: System::Windows::Forms::Label *  label7;
	private: System::Windows::Forms::Label *  label8;
	private: System::Windows::Forms::Label *  label9;
	public: System::Windows::Forms::TextBox *  textBox1;
	public: System::Windows::Forms::TextBox *  textBox2;
	public: System::Windows::Forms::TextBox *  textBox3;
	public: System::Windows::Forms::TextBox *  textBox4;
	public: System::Windows::Forms::TextBox *  textBox5;
	public: System::Windows::Forms::TextBox *  textBox6;
	public: System::Windows::Forms::TextBox *  textBox7;
	private: System::Windows::Forms::Label *  label10;
	private: System::Windows::Forms::Label *  label11;
	private: System::Windows::Forms::Label *  label12;
	public: System::Windows::Forms::TextBox *  textBox8;
	public: System::Windows::Forms::TextBox *  textBox9;
	public: System::Windows::Forms::TextBox *  textBox10;
	private: System::Windows::Forms::Label *  label13;
	private: System::Windows::Forms::Label *  label14;
	public: System::Windows::Forms::TextBox *  textBox11;
	public: System::Windows::Forms::TextBox *  textBox12;
	private: System::Windows::Forms::GroupBox *  groupBox1;
	private: System::Windows::Forms::GroupBox *  groupBox2;
	private: System::Windows::Forms::Label *  label15;
	private: System::Windows::Forms::Label *  label16;
	public: System::Windows::Forms::TextBox *  textBox13;
	private: System::Windows::Forms::Label *  label17;
	public: System::Windows::Forms::TextBox *  textBox14;
	public: System::Windows::Forms::TextBox *  textBox15;
	private: System::Windows::Forms::GroupBox *  groupBox3;
	private: System::Windows::Forms::Label *  label18;
	private: System::Windows::Forms::Label *  label19;
	public: System::Windows::Forms::TextBox *  textBox16;
	private: System::Windows::Forms::Label *  label20;
	public: System::Windows::Forms::TextBox *  textBox17;
	public: System::Windows::Forms::TextBox *  textBox18;
	private: System::Windows::Forms::GroupBox *  groupBox4;
	private: System::Windows::Forms::GroupBox *  groupBox5;
	private: System::Windows::Forms::Label *  label21;
	public: System::Windows::Forms::TextBox *  textBox19;
	private: System::Windows::Forms::Label *  label22;
	private: System::Windows::Forms::Label *  label23;
	public: System::Windows::Forms::TextBox *  textBox20;
	private: System::Windows::Forms::Label *  label24;
	public: System::Windows::Forms::TextBox *  textBox21;
	public: System::Windows::Forms::TextBox *  textBox22;
	private: System::Windows::Forms::GroupBox *  groupBox6;
	private: System::Windows::Forms::Label *  label25;
	public: System::Windows::Forms::TextBox *  textBox23;
	private: System::Windows::Forms::Label *  label26;
	public: System::Windows::Forms::TextBox *  textBox24;
	private: System::Windows::Forms::Label *  label27;
	public: System::Windows::Forms::TextBox *  textBox25;
	private: System::Windows::Forms::Label *  label28;
	public: System::Windows::Forms::TextBox *  textBox26;
	private: System::Windows::Forms::GroupBox *  groupBox7;
	private: System::Windows::Forms::Label *  label29;
	public: System::Windows::Forms::TextBox *  textBox27;
	private: System::Windows::Forms::Label *  label30;
	public: System::Windows::Forms::TextBox *  textBox28;
	public: System::Windows::Forms::TextBox *  textBox29;
	public: System::Windows::Forms::TextBox *  textBox30;
	private: System::Windows::Forms::Label *  label31;
	private: System::Windows::Forms::Label *  label32;
	public: System::Windows::Forms::TextBox *  textBox31;
	private: System::Windows::Forms::Label *  label33;
	public: System::Windows::Forms::TextBox *  textBox32;
	public: System::Windows::Forms::TextBox *  textBox33;
	private: System::Windows::Forms::Label *  label34;
	private: System::Windows::Forms::Label *  label35;
	private: System::Windows::Forms::Label *  label36;
	public: System::Windows::Forms::TextBox *  textBox34;
	private: System::Windows::Forms::GroupBox *  groupBox8;
	public: System::Windows::Forms::TextBox *  textBox35;
	private: System::Windows::Forms::Label *  label37;
	public: System::Windows::Forms::TextBox *  textBox36;
	private: System::Windows::Forms::Label *  label38;
	public: System::Windows::Forms::TextBox *  textBox37;
	public: System::Windows::Forms::TextBox *  textBox38;
	private: System::Windows::Forms::Label *  label39;
	public: System::Windows::Forms::TextBox *  textBox39;
	public: System::Windows::Forms::TextBox *  textBox40;
	public: System::Windows::Forms::TextBox *  textBox41;
	private: System::Windows::Forms::Label *  label40;
	private: System::Windows::Forms::Label *  label41;
	private: System::Windows::Forms::Label *  label42;
	private: System::Windows::Forms::Label *  label43;
	private: System::Windows::Forms::Label *  label44;
	public: System::Windows::Forms::TextBox *  textBox42;
	private: System::Windows::Forms::Label *  label45;
	public: System::Windows::Forms::TextBox *  textBox43;
	private: System::Windows::Forms::Label *  label46;
	public: System::Windows::Forms::TextBox *  textBox44;
	private: System::Windows::Forms::Label *  label47;
	public: System::Windows::Forms::TextBox *  textBox45;
	private: System::Windows::Forms::GroupBox *  groupBox9;
	public: System::Windows::Forms::TextBox *  textBox46;
	private: System::Windows::Forms::Label *  label48;
	private: System::Windows::Forms::Label *  label49;
	public: System::Windows::Forms::TextBox *  textBox47;
	private: System::Windows::Forms::Label *  label50;
	public: System::Windows::Forms::TextBox *  textBox48;
	private: System::Windows::Forms::GroupBox *  groupBox10;
	private: System::Windows::Forms::Label *  label51;
	public: System::Windows::Forms::TextBox *  textBox49;
	private: System::Windows::Forms::Label *  label52;
	public: System::Windows::Forms::TextBox *  textBox50;
	private: System::Windows::Forms::Label *  label53;
	public: System::Windows::Forms::TextBox *  textBox51;
	private: System::Windows::Forms::Label *  label54;
	public: System::Windows::Forms::TextBox *  textBox52;
	private: System::Windows::Forms::Label *  label55;
	public: System::Windows::Forms::TextBox *  textBox53;
	private: System::Windows::Forms::Label *  label56;
	public: System::Windows::Forms::TextBox *  textBox54;
private: System::Windows::Forms::Label *  label57;
public: System::Windows::Forms::TextBox *  textBox55;
private: System::Windows::Forms::Label *  label58;
public: System::Windows::Forms::TextBox *  textBox56;
private: System::Windows::Forms::Label *  label59;
public: System::Windows::Forms::TextBox *  textBox57;
private: System::Windows::Forms::Label *  label60;
public: System::Windows::Forms::TextBox *  textBox58;
private: System::Windows::Forms::Label *  label61;
public: System::Windows::Forms::TextBox *  textBox59;
private: System::Windows::Forms::Label *  label62;
public: System::Windows::Forms::TextBox *  textBox60;
private: System::Windows::Forms::Label *  label63;
public: System::Windows::Forms::TextBox *  textBox61;
private: System::Windows::Forms::Label *  label64;
public: System::Windows::Forms::TextBox *  textBox62;
private: System::Windows::Forms::Label *  label65;
public: System::Windows::Forms::TextBox *  textBox63;
private: System::Windows::Forms::Label *  label66;
public: System::Windows::Forms::TextBox *  textBox64;

























private: System::Windows::Forms::Label *  label79;
public: System::Windows::Forms::TextBox *  textBox77;
private: System::Windows::Forms::Label *  label80;
public: System::Windows::Forms::TextBox *  textBox78;
private: System::Windows::Forms::GroupBox *  groupBox12;
private: System::Windows::Forms::Label *  label81;
public: System::Windows::Forms::TextBox *  textBox79;
private: System::Windows::Forms::Label *  label82;
public: System::Windows::Forms::TextBox *  textBox80;
private: System::Windows::Forms::Label *  label83;
public: System::Windows::Forms::TextBox *  textBox81;
private: System::Windows::Forms::Label *  label84;
public: System::Windows::Forms::TextBox *  textBox82;
private: System::Windows::Forms::Label *  label85;
public: System::Windows::Forms::TextBox *  textBox83;
private: System::Windows::Forms::Label *  label86;
public: System::Windows::Forms::TextBox *  textBox84;
private: System::Windows::Forms::Label *  label87;
public: System::Windows::Forms::TextBox *  textBox85;
private: System::Windows::Forms::Label *  label88;
public: System::Windows::Forms::TextBox *  textBox86;
private: System::Windows::Forms::Label *  label89;
public: System::Windows::Forms::TextBox *  textBox87;
private: System::Windows::Forms::Label *  label90;
public: System::Windows::Forms::TextBox *  textBox88;
private: System::Windows::Forms::Label *  label91;
public: System::Windows::Forms::TextBox *  textBox89;
private: System::Windows::Forms::Label *  label92;
public: System::Windows::Forms::TextBox *  textBox90;
private: System::Windows::Forms::GroupBox *  groupBox13;
private: System::Windows::Forms::Label *  label93;
private: System::Windows::Forms::Label *  label94;
public: System::Windows::Forms::TextBox *  textBox91;
private: System::Windows::Forms::Label *  label95;
public: System::Windows::Forms::TextBox *  textBox92;
public: System::Windows::Forms::TextBox *  textBox93;
private: System::Windows::Forms::GroupBox *  groupBox14;
public: System::Windows::Forms::TextBox *  textBox94;
private: System::Windows::Forms::Label *  label96;
private: System::Windows::Forms::Label *  label97;
public: System::Windows::Forms::TextBox *  textBox95;
private: System::Windows::Forms::Label *  label98;
public: System::Windows::Forms::TextBox *  textBox96;
private: System::Windows::Forms::GroupBox *  groupBox15;
public: System::Windows::Forms::TextBox *  textBox97;
private: System::Windows::Forms::Label *  label99;
private: System::Windows::Forms::Label *  label100;
public: System::Windows::Forms::TextBox *  textBox98;
private: System::Windows::Forms::Label *  label101;
public: System::Windows::Forms::TextBox *  textBox99;
private: System::Windows::Forms::Label *  label102;
public: System::Windows::Forms::TextBox *  textBox100;
private: System::Windows::Forms::Label *  label103;
public: System::Windows::Forms::TextBox *  textBox101;
private: System::Windows::Forms::Label *  label104;
public: System::Windows::Forms::TextBox *  textBox102;
private: System::Windows::Forms::Label *  label105;
public: System::Windows::Forms::TextBox *  textBox103;
private: System::Windows::Forms::Label *  label106;
public: System::Windows::Forms::TextBox *  textBox104;
private: System::Windows::Forms::Label *  label107;
public: System::Windows::Forms::TextBox *  textBox105;
private: System::Windows::Forms::Label *  label108;
public: System::Windows::Forms::TextBox *  textBox106;
private: System::Windows::Forms::Label *  label109;
public: System::Windows::Forms::TextBox *  textBox107;
private: System::Windows::Forms::Label *  label110;
public: System::Windows::Forms::TextBox *  textBox108;
private: System::Windows::Forms::GroupBox *  groupBox16;
public: System::Windows::Forms::TextBox *  textBox109;
private: System::Windows::Forms::Label *  label111;
private: System::Windows::Forms::Label *  label112;
public: System::Windows::Forms::TextBox *  textBox110;
private: System::Windows::Forms::Label *  label113;
public: System::Windows::Forms::TextBox *  textBox111;
private: System::Windows::Forms::GroupBox *  groupBox17;
private: System::Windows::Forms::Label *  label114;
private: System::Windows::Forms::Label *  label115;
public: System::Windows::Forms::TextBox *  textBox112;
private: System::Windows::Forms::Label *  label116;
public: System::Windows::Forms::TextBox *  textBox113;
public: System::Windows::Forms::TextBox *  textBox114;
private: System::Windows::Forms::Label *  label117;
public: System::Windows::Forms::TextBox *  textBox115;
private: System::Windows::Forms::GroupBox *  groupBox18;
public: System::Windows::Forms::TextBox *  textBox116;
private: System::Windows::Forms::Label *  label118;
private: System::Windows::Forms::Label *  label119;
public: System::Windows::Forms::TextBox *  textBox117;
private: System::Windows::Forms::Label *  label120;
public: System::Windows::Forms::TextBox *  textBox118;
private: System::Windows::Forms::GroupBox *  groupBox19;
private: System::Windows::Forms::Label *  label121;
public: System::Windows::Forms::TextBox *  textBox119;
private: System::Windows::Forms::Label *  label122;
public: System::Windows::Forms::TextBox *  textBox120;
private: System::Windows::Forms::Label *  label123;
public: System::Windows::Forms::TextBox *  textBox121;
private: System::Windows::Forms::Label *  label124;
public: System::Windows::Forms::TextBox *  textBox122;
private: System::Windows::Forms::Label *  label125;
public: System::Windows::Forms::TextBox *  textBox123;
private: System::Windows::Forms::Label *  label126;
public: System::Windows::Forms::TextBox *  textBox124;
private: System::Windows::Forms::Label *  label127;
public: System::Windows::Forms::TextBox *  textBox125;
private: System::Windows::Forms::Label *  label128;
public: System::Windows::Forms::TextBox *  textBox126;
private: System::Windows::Forms::Label *  label129;
public: System::Windows::Forms::TextBox *  textBox127;
private: System::Windows::Forms::Label *  label130;
public: System::Windows::Forms::TextBox *  textBox128;



private: System::Windows::Forms::GroupBox *  groupBox21;
public: System::Windows::Forms::TextBox *  textBox130;
private: System::Windows::Forms::Label *  label132;
private: System::Windows::Forms::Label *  label133;
public: System::Windows::Forms::TextBox *  textBox131;
private: System::Windows::Forms::Label *  label134;
public: System::Windows::Forms::TextBox *  textBox132;
private: System::Windows::Forms::GroupBox *  groupBox20;
private: System::Windows::Forms::Label *  label2;
public: System::Windows::Forms::TextBox *  textBox129;
public: System::Windows::Forms::TextBox *  textBox133;
private: System::Windows::Forms::Label *  label131;
public: System::Windows::Forms::TextBox *  textBox134;
private: System::Windows::Forms::Label *  label135;
public: System::Windows::Forms::TextBox *  textBox135;
private: System::Windows::Forms::Label *  label136;
public: System::Windows::Forms::TextBox *  textBox136;
private: System::Windows::Forms::Label *  label137;
public: System::Windows::Forms::TextBox *  textBox137;
private: System::Windows::Forms::Label *  label138;


public: System::Windows::Forms::Label *  label69;
public: System::Windows::Forms::Label *  label70;

public: System::Windows::Forms::Label *  label72;


public: System::Windows::Forms::Label *  label75;

private: System::Windows::Forms::GroupBox *  groupBox11;
private: System::Windows::Forms::Label *  label68;
public: System::Windows::Forms::TextBox *  textBox65;
private: System::Windows::Forms::Label *  label71;
public: System::Windows::Forms::TextBox *  textBox66;
private: System::Windows::Forms::Label *  label73;
public: System::Windows::Forms::TextBox *  textBox67;
public: System::Windows::Forms::TextBox *  textBox68;
private: System::Windows::Forms::Label *  label74;
public: System::Windows::Forms::TextBox *  textBox69;
private: System::Windows::Forms::Label *  label76;
public: System::Windows::Forms::TextBox *  textBox70;
private: System::Windows::Forms::Label *  label77;
private: System::Windows::Forms::Label *  label67;
public: System::Windows::Forms::TextBox *  textBox71;
private: System::Windows::Forms::Label *  label78;
public: System::Windows::Forms::TextBox *  textBox72;
public: System::Windows::Forms::TextBox *  textBox73;
private: System::Windows::Forms::Label *  label139;
public: System::Windows::Forms::TextBox *  textBox74;
private: System::Windows::Forms::Label *  label140;
private: System::Windows::Forms::GroupBox *  groupBox22;
public: System::Windows::Forms::TextBox *  textBox75;
private: System::Windows::Forms::Label *  label141;
private: System::Windows::Forms::Label *  label142;
public: System::Windows::Forms::TextBox *  textBox76;
private: System::Windows::Forms::Label *  label143;
public: System::Windows::Forms::TextBox *  textBox138;
public: System::Windows::Forms::TextBox *  textBox139;
private: System::Windows::Forms::Label *  label144;
public: System::Windows::Forms::TextBox *  textBox140;
private: System::Windows::Forms::Label *  label145;
public: System::Windows::Forms::TextBox *  textBox141;
private: System::Windows::Forms::Label *  label146;
public: System::Windows::Forms::TextBox *  textBox142;
private: System::Windows::Forms::Label *  label147;
public: System::Windows::Forms::TextBox *  textBox143;
private: System::Windows::Forms::Label *  label148;
private: System::Windows::Forms::GroupBox *  groupBox23;
public: System::Windows::Forms::TextBox *  textBox144;
private: System::Windows::Forms::Label *  label149;
private: System::Windows::Forms::Label *  label150;
public: System::Windows::Forms::TextBox *  textBox145;
private: System::Windows::Forms::Label *  label151;
public: System::Windows::Forms::TextBox *  textBox146;
private: System::Windows::Forms::GroupBox *  groupBox24;
public: System::Windows::Forms::TextBox *  textBox147;
private: System::Windows::Forms::Label *  label152;
public: System::Windows::Forms::TextBox *  textBox148;
private: System::Windows::Forms::Label *  label153;
public: System::Windows::Forms::TextBox *  textBox149;
private: System::Windows::Forms::Label *  label154;
public: System::Windows::Forms::TextBox *  textBox150;
private: System::Windows::Forms::Label *  label155;
public: System::Windows::Forms::TextBox *  textBox151;
private: System::Windows::Forms::Label *  label156;
private: System::Windows::Forms::Label *  label157;
public: System::Windows::Forms::TextBox *  textBox152;
private: System::Windows::Forms::Label *  label158;
public: System::Windows::Forms::TextBox *  textBox153;
private: System::Windows::Forms::GroupBox *  groupBox25;
public: System::Windows::Forms::TextBox *  textBox154;
private: System::Windows::Forms::Label *  label159;
private: System::Windows::Forms::Label *  label160;
public: System::Windows::Forms::TextBox *  textBox155;
private: System::Windows::Forms::Label *  label161;
public: System::Windows::Forms::TextBox *  textBox156;
public: System::Windows::Forms::TextBox *  textBox157;
private: System::Windows::Forms::Label *  label162;
public: System::Windows::Forms::TextBox *  textBox158;
private: System::Windows::Forms::Label *  label163;
public: System::Windows::Forms::TextBox *  textBox159;
private: System::Windows::Forms::Label *  label164;
public: System::Windows::Forms::TextBox *  textBox160;
private: System::Windows::Forms::Label *  label165;
private: System::Windows::Forms::GroupBox *  groupBox26;
public: System::Windows::Forms::TextBox *  textBox161;
private: System::Windows::Forms::Label *  label166;
private: System::Windows::Forms::Label *  label167;
public: System::Windows::Forms::TextBox *  textBox162;
private: System::Windows::Forms::Label *  label168;
public: System::Windows::Forms::TextBox *  textBox163;
public: System::Windows::Forms::TextBox *  textBox164;
private: System::Windows::Forms::Label *  label169;
public: System::Windows::Forms::TextBox *  textBox165;
private: System::Windows::Forms::Label *  label170;
public: System::Windows::Forms::TextBox *  textBox166;
private: System::Windows::Forms::Label *  label171;
public: System::Windows::Forms::TextBox *  textBox167;
private: System::Windows::Forms::Label *  label172;
public: System::Windows::Forms::TextBox *  textBox168;
private: System::Windows::Forms::Label *  label173;
public: System::Windows::Forms::TextBox *  textBox169;
private: System::Windows::Forms::Label *  label174;
public: System::Windows::Forms::TextBox *  textBox170;
private: System::Windows::Forms::Label *  label175;
public: System::Windows::Forms::TextBox *  textBox171;
private: System::Windows::Forms::Label *  label176;
private: System::Windows::Forms::GroupBox *  groupBox27;
public: System::Windows::Forms::TextBox *  textBox172;
private: System::Windows::Forms::Label *  label177;
private: System::Windows::Forms::Label *  label178;
public: System::Windows::Forms::TextBox *  textBox173;
private: System::Windows::Forms::Label *  label179;
public: System::Windows::Forms::TextBox *  textBox174;
public: System::Windows::Forms::TextBox *  textBox175;
private: System::Windows::Forms::Label *  label180;
public: System::Windows::Forms::TextBox *  textBox176;
private: System::Windows::Forms::Label *  label181;
public: System::Windows::Forms::TextBox *  textBox177;
private: System::Windows::Forms::Label *  label182;
public: System::Windows::Forms::TextBox *  textBox178;
private: System::Windows::Forms::Label *  label183;
public: System::Windows::Forms::TextBox *  textBox179;
private: System::Windows::Forms::Label *  label184;






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
			this->cmcListID = new System::Windows::Forms::TextBox();
			this->label3 = new System::Windows::Forms::Label();
			this->label4 = new System::Windows::Forms::Label();
			this->label5 = new System::Windows::Forms::Label();
			this->label6 = new System::Windows::Forms::Label();
			this->label7 = new System::Windows::Forms::Label();
			this->label8 = new System::Windows::Forms::Label();
			this->label9 = new System::Windows::Forms::Label();
			this->textBox1 = new System::Windows::Forms::TextBox();
			this->textBox2 = new System::Windows::Forms::TextBox();
			this->textBox3 = new System::Windows::Forms::TextBox();
			this->textBox4 = new System::Windows::Forms::TextBox();
			this->textBox5 = new System::Windows::Forms::TextBox();
			this->textBox6 = new System::Windows::Forms::TextBox();
			this->textBox7 = new System::Windows::Forms::TextBox();
			this->label10 = new System::Windows::Forms::Label();
			this->label11 = new System::Windows::Forms::Label();
			this->label12 = new System::Windows::Forms::Label();
			this->textBox8 = new System::Windows::Forms::TextBox();
			this->textBox9 = new System::Windows::Forms::TextBox();
			this->textBox10 = new System::Windows::Forms::TextBox();
			this->label13 = new System::Windows::Forms::Label();
			this->label14 = new System::Windows::Forms::Label();
			this->textBox11 = new System::Windows::Forms::TextBox();
			this->textBox12 = new System::Windows::Forms::TextBox();
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->groupBox2 = new System::Windows::Forms::GroupBox();
			this->label15 = new System::Windows::Forms::Label();
			this->label16 = new System::Windows::Forms::Label();
			this->textBox13 = new System::Windows::Forms::TextBox();
			this->label17 = new System::Windows::Forms::Label();
			this->textBox14 = new System::Windows::Forms::TextBox();
			this->textBox15 = new System::Windows::Forms::TextBox();
			this->groupBox3 = new System::Windows::Forms::GroupBox();
			this->label18 = new System::Windows::Forms::Label();
			this->label19 = new System::Windows::Forms::Label();
			this->textBox16 = new System::Windows::Forms::TextBox();
			this->label20 = new System::Windows::Forms::Label();
			this->textBox17 = new System::Windows::Forms::TextBox();
			this->textBox18 = new System::Windows::Forms::TextBox();
			this->groupBox4 = new System::Windows::Forms::GroupBox();
			this->groupBox5 = new System::Windows::Forms::GroupBox();
			this->label110 = new System::Windows::Forms::Label();
			this->textBox108 = new System::Windows::Forms::TextBox();
			this->label109 = new System::Windows::Forms::Label();
			this->textBox107 = new System::Windows::Forms::TextBox();
			this->label105 = new System::Windows::Forms::Label();
			this->textBox103 = new System::Windows::Forms::TextBox();
			this->label102 = new System::Windows::Forms::Label();
			this->textBox100 = new System::Windows::Forms::TextBox();
			this->label89 = new System::Windows::Forms::Label();
			this->textBox87 = new System::Windows::Forms::TextBox();
			this->label66 = new System::Windows::Forms::Label();
			this->textBox64 = new System::Windows::Forms::TextBox();
			this->label63 = new System::Windows::Forms::Label();
			this->textBox61 = new System::Windows::Forms::TextBox();
			this->label64 = new System::Windows::Forms::Label();
			this->textBox62 = new System::Windows::Forms::TextBox();
			this->label65 = new System::Windows::Forms::Label();
			this->textBox63 = new System::Windows::Forms::TextBox();
			this->label61 = new System::Windows::Forms::Label();
			this->textBox59 = new System::Windows::Forms::TextBox();
			this->label62 = new System::Windows::Forms::Label();
			this->textBox60 = new System::Windows::Forms::TextBox();
			this->label59 = new System::Windows::Forms::Label();
			this->textBox57 = new System::Windows::Forms::TextBox();
			this->label60 = new System::Windows::Forms::Label();
			this->textBox58 = new System::Windows::Forms::TextBox();
			this->label58 = new System::Windows::Forms::Label();
			this->textBox56 = new System::Windows::Forms::TextBox();
			this->label57 = new System::Windows::Forms::Label();
			this->textBox55 = new System::Windows::Forms::TextBox();
			this->label21 = new System::Windows::Forms::Label();
			this->textBox19 = new System::Windows::Forms::TextBox();
			this->label22 = new System::Windows::Forms::Label();
			this->label23 = new System::Windows::Forms::Label();
			this->textBox20 = new System::Windows::Forms::TextBox();
			this->label24 = new System::Windows::Forms::Label();
			this->textBox21 = new System::Windows::Forms::TextBox();
			this->textBox22 = new System::Windows::Forms::TextBox();
			this->groupBox6 = new System::Windows::Forms::GroupBox();
			this->label25 = new System::Windows::Forms::Label();
			this->textBox23 = new System::Windows::Forms::TextBox();
			this->label26 = new System::Windows::Forms::Label();
			this->textBox24 = new System::Windows::Forms::TextBox();
			this->label27 = new System::Windows::Forms::Label();
			this->textBox25 = new System::Windows::Forms::TextBox();
			this->label28 = new System::Windows::Forms::Label();
			this->textBox26 = new System::Windows::Forms::TextBox();
			this->groupBox7 = new System::Windows::Forms::GroupBox();
			this->textBox28 = new System::Windows::Forms::TextBox();
			this->label30 = new System::Windows::Forms::Label();
			this->textBox27 = new System::Windows::Forms::TextBox();
			this->label29 = new System::Windows::Forms::Label();
			this->textBox33 = new System::Windows::Forms::TextBox();
			this->textBox32 = new System::Windows::Forms::TextBox();
			this->label33 = new System::Windows::Forms::Label();
			this->textBox31 = new System::Windows::Forms::TextBox();
			this->textBox29 = new System::Windows::Forms::TextBox();
			this->textBox30 = new System::Windows::Forms::TextBox();
			this->label31 = new System::Windows::Forms::Label();
			this->label32 = new System::Windows::Forms::Label();
			this->label35 = new System::Windows::Forms::Label();
			this->label34 = new System::Windows::Forms::Label();
			this->label36 = new System::Windows::Forms::Label();
			this->textBox34 = new System::Windows::Forms::TextBox();
			this->groupBox8 = new System::Windows::Forms::GroupBox();
			this->textBox35 = new System::Windows::Forms::TextBox();
			this->label37 = new System::Windows::Forms::Label();
			this->textBox36 = new System::Windows::Forms::TextBox();
			this->label38 = new System::Windows::Forms::Label();
			this->textBox37 = new System::Windows::Forms::TextBox();
			this->textBox38 = new System::Windows::Forms::TextBox();
			this->label39 = new System::Windows::Forms::Label();
			this->textBox39 = new System::Windows::Forms::TextBox();
			this->textBox40 = new System::Windows::Forms::TextBox();
			this->textBox41 = new System::Windows::Forms::TextBox();
			this->label40 = new System::Windows::Forms::Label();
			this->label41 = new System::Windows::Forms::Label();
			this->label42 = new System::Windows::Forms::Label();
			this->label43 = new System::Windows::Forms::Label();
			this->label44 = new System::Windows::Forms::Label();
			this->textBox42 = new System::Windows::Forms::TextBox();
			this->label45 = new System::Windows::Forms::Label();
			this->textBox43 = new System::Windows::Forms::TextBox();
			this->label46 = new System::Windows::Forms::Label();
			this->textBox44 = new System::Windows::Forms::TextBox();
			this->label47 = new System::Windows::Forms::Label();
			this->textBox45 = new System::Windows::Forms::TextBox();
			this->groupBox9 = new System::Windows::Forms::GroupBox();
			this->textBox46 = new System::Windows::Forms::TextBox();
			this->label48 = new System::Windows::Forms::Label();
			this->label49 = new System::Windows::Forms::Label();
			this->textBox47 = new System::Windows::Forms::TextBox();
			this->label50 = new System::Windows::Forms::Label();
			this->textBox48 = new System::Windows::Forms::TextBox();
			this->groupBox10 = new System::Windows::Forms::GroupBox();
			this->label51 = new System::Windows::Forms::Label();
			this->textBox49 = new System::Windows::Forms::TextBox();
			this->label52 = new System::Windows::Forms::Label();
			this->textBox50 = new System::Windows::Forms::TextBox();
			this->label53 = new System::Windows::Forms::Label();
			this->textBox51 = new System::Windows::Forms::TextBox();
			this->label54 = new System::Windows::Forms::Label();
			this->textBox52 = new System::Windows::Forms::TextBox();
			this->label55 = new System::Windows::Forms::Label();
			this->textBox53 = new System::Windows::Forms::TextBox();
			this->label56 = new System::Windows::Forms::Label();
			this->textBox54 = new System::Windows::Forms::TextBox();
			this->label79 = new System::Windows::Forms::Label();
			this->textBox77 = new System::Windows::Forms::TextBox();
			this->label80 = new System::Windows::Forms::Label();
			this->textBox78 = new System::Windows::Forms::TextBox();
			this->groupBox12 = new System::Windows::Forms::GroupBox();
			this->label81 = new System::Windows::Forms::Label();
			this->textBox79 = new System::Windows::Forms::TextBox();
			this->label82 = new System::Windows::Forms::Label();
			this->textBox80 = new System::Windows::Forms::TextBox();
			this->label83 = new System::Windows::Forms::Label();
			this->textBox81 = new System::Windows::Forms::TextBox();
			this->label84 = new System::Windows::Forms::Label();
			this->textBox82 = new System::Windows::Forms::TextBox();
			this->label85 = new System::Windows::Forms::Label();
			this->textBox83 = new System::Windows::Forms::TextBox();
			this->label86 = new System::Windows::Forms::Label();
			this->textBox84 = new System::Windows::Forms::TextBox();
			this->label87 = new System::Windows::Forms::Label();
			this->textBox85 = new System::Windows::Forms::TextBox();
			this->label88 = new System::Windows::Forms::Label();
			this->textBox86 = new System::Windows::Forms::TextBox();
			this->label90 = new System::Windows::Forms::Label();
			this->textBox88 = new System::Windows::Forms::TextBox();
			this->label91 = new System::Windows::Forms::Label();
			this->textBox89 = new System::Windows::Forms::TextBox();
			this->label92 = new System::Windows::Forms::Label();
			this->textBox90 = new System::Windows::Forms::TextBox();
			this->groupBox13 = new System::Windows::Forms::GroupBox();
			this->label158 = new System::Windows::Forms::Label();
			this->textBox153 = new System::Windows::Forms::TextBox();
			this->label93 = new System::Windows::Forms::Label();
			this->label94 = new System::Windows::Forms::Label();
			this->textBox91 = new System::Windows::Forms::TextBox();
			this->label95 = new System::Windows::Forms::Label();
			this->textBox92 = new System::Windows::Forms::TextBox();
			this->textBox93 = new System::Windows::Forms::TextBox();
			this->groupBox14 = new System::Windows::Forms::GroupBox();
			this->textBox94 = new System::Windows::Forms::TextBox();
			this->label96 = new System::Windows::Forms::Label();
			this->label97 = new System::Windows::Forms::Label();
			this->textBox95 = new System::Windows::Forms::TextBox();
			this->label98 = new System::Windows::Forms::Label();
			this->textBox96 = new System::Windows::Forms::TextBox();
			this->groupBox15 = new System::Windows::Forms::GroupBox();
			this->textBox97 = new System::Windows::Forms::TextBox();
			this->label99 = new System::Windows::Forms::Label();
			this->label100 = new System::Windows::Forms::Label();
			this->textBox98 = new System::Windows::Forms::TextBox();
			this->label101 = new System::Windows::Forms::Label();
			this->textBox99 = new System::Windows::Forms::TextBox();
			this->label103 = new System::Windows::Forms::Label();
			this->textBox101 = new System::Windows::Forms::TextBox();
			this->label104 = new System::Windows::Forms::Label();
			this->textBox102 = new System::Windows::Forms::TextBox();
			this->label106 = new System::Windows::Forms::Label();
			this->textBox104 = new System::Windows::Forms::TextBox();
			this->label107 = new System::Windows::Forms::Label();
			this->textBox105 = new System::Windows::Forms::TextBox();
			this->label108 = new System::Windows::Forms::Label();
			this->textBox106 = new System::Windows::Forms::TextBox();
			this->groupBox16 = new System::Windows::Forms::GroupBox();
			this->textBox109 = new System::Windows::Forms::TextBox();
			this->label111 = new System::Windows::Forms::Label();
			this->label112 = new System::Windows::Forms::Label();
			this->textBox110 = new System::Windows::Forms::TextBox();
			this->label113 = new System::Windows::Forms::Label();
			this->textBox111 = new System::Windows::Forms::TextBox();
			this->groupBox17 = new System::Windows::Forms::GroupBox();
			this->label114 = new System::Windows::Forms::Label();
			this->label115 = new System::Windows::Forms::Label();
			this->textBox112 = new System::Windows::Forms::TextBox();
			this->label116 = new System::Windows::Forms::Label();
			this->textBox113 = new System::Windows::Forms::TextBox();
			this->textBox114 = new System::Windows::Forms::TextBox();
			this->label117 = new System::Windows::Forms::Label();
			this->textBox115 = new System::Windows::Forms::TextBox();
			this->groupBox18 = new System::Windows::Forms::GroupBox();
			this->textBox116 = new System::Windows::Forms::TextBox();
			this->label118 = new System::Windows::Forms::Label();
			this->label119 = new System::Windows::Forms::Label();
			this->textBox117 = new System::Windows::Forms::TextBox();
			this->label120 = new System::Windows::Forms::Label();
			this->textBox118 = new System::Windows::Forms::TextBox();
			this->groupBox19 = new System::Windows::Forms::GroupBox();
			this->label129 = new System::Windows::Forms::Label();
			this->textBox127 = new System::Windows::Forms::TextBox();
			this->label130 = new System::Windows::Forms::Label();
			this->textBox128 = new System::Windows::Forms::TextBox();
			this->label128 = new System::Windows::Forms::Label();
			this->textBox126 = new System::Windows::Forms::TextBox();
			this->label127 = new System::Windows::Forms::Label();
			this->textBox125 = new System::Windows::Forms::TextBox();
			this->label126 = new System::Windows::Forms::Label();
			this->textBox124 = new System::Windows::Forms::TextBox();
			this->label125 = new System::Windows::Forms::Label();
			this->textBox123 = new System::Windows::Forms::TextBox();
			this->label124 = new System::Windows::Forms::Label();
			this->textBox122 = new System::Windows::Forms::TextBox();
			this->label123 = new System::Windows::Forms::Label();
			this->textBox121 = new System::Windows::Forms::TextBox();
			this->label122 = new System::Windows::Forms::Label();
			this->textBox120 = new System::Windows::Forms::TextBox();
			this->label121 = new System::Windows::Forms::Label();
			this->textBox119 = new System::Windows::Forms::TextBox();
			this->groupBox21 = new System::Windows::Forms::GroupBox();
			this->textBox130 = new System::Windows::Forms::TextBox();
			this->label132 = new System::Windows::Forms::Label();
			this->label133 = new System::Windows::Forms::Label();
			this->textBox131 = new System::Windows::Forms::TextBox();
			this->label134 = new System::Windows::Forms::Label();
			this->textBox132 = new System::Windows::Forms::TextBox();
			this->groupBox20 = new System::Windows::Forms::GroupBox();
			this->label75 = new System::Windows::Forms::Label();
			this->label70 = new System::Windows::Forms::Label();
			this->label69 = new System::Windows::Forms::Label();
			this->textBox137 = new System::Windows::Forms::TextBox();
			this->label138 = new System::Windows::Forms::Label();
			this->textBox136 = new System::Windows::Forms::TextBox();
			this->label137 = new System::Windows::Forms::Label();
			this->textBox135 = new System::Windows::Forms::TextBox();
			this->label136 = new System::Windows::Forms::Label();
			this->textBox134 = new System::Windows::Forms::TextBox();
			this->label135 = new System::Windows::Forms::Label();
			this->textBox133 = new System::Windows::Forms::TextBox();
			this->label131 = new System::Windows::Forms::Label();
			this->textBox129 = new System::Windows::Forms::TextBox();
			this->label2 = new System::Windows::Forms::Label();
			this->label72 = new System::Windows::Forms::Label();
			this->groupBox11 = new System::Windows::Forms::GroupBox();
			this->textBox158 = new System::Windows::Forms::TextBox();
			this->label163 = new System::Windows::Forms::Label();
			this->textBox159 = new System::Windows::Forms::TextBox();
			this->label164 = new System::Windows::Forms::Label();
			this->label157 = new System::Windows::Forms::Label();
			this->textBox152 = new System::Windows::Forms::TextBox();
			this->textBox141 = new System::Windows::Forms::TextBox();
			this->label146 = new System::Windows::Forms::Label();
			this->textBox142 = new System::Windows::Forms::TextBox();
			this->label147 = new System::Windows::Forms::Label();
			this->textBox143 = new System::Windows::Forms::TextBox();
			this->label148 = new System::Windows::Forms::Label();
			this->textBox139 = new System::Windows::Forms::TextBox();
			this->label144 = new System::Windows::Forms::Label();
			this->textBox140 = new System::Windows::Forms::TextBox();
			this->label145 = new System::Windows::Forms::Label();
			this->textBox74 = new System::Windows::Forms::TextBox();
			this->label140 = new System::Windows::Forms::Label();
			this->textBox73 = new System::Windows::Forms::TextBox();
			this->label139 = new System::Windows::Forms::Label();
			this->textBox68 = new System::Windows::Forms::TextBox();
			this->label74 = new System::Windows::Forms::Label();
			this->textBox69 = new System::Windows::Forms::TextBox();
			this->label76 = new System::Windows::Forms::Label();
			this->textBox70 = new System::Windows::Forms::TextBox();
			this->label77 = new System::Windows::Forms::Label();
			this->textBox67 = new System::Windows::Forms::TextBox();
			this->label73 = new System::Windows::Forms::Label();
			this->textBox66 = new System::Windows::Forms::TextBox();
			this->label71 = new System::Windows::Forms::Label();
			this->textBox65 = new System::Windows::Forms::TextBox();
			this->label68 = new System::Windows::Forms::Label();
			this->label67 = new System::Windows::Forms::Label();
			this->textBox71 = new System::Windows::Forms::TextBox();
			this->label78 = new System::Windows::Forms::Label();
			this->textBox72 = new System::Windows::Forms::TextBox();
			this->groupBox22 = new System::Windows::Forms::GroupBox();
			this->textBox75 = new System::Windows::Forms::TextBox();
			this->label141 = new System::Windows::Forms::Label();
			this->label142 = new System::Windows::Forms::Label();
			this->textBox76 = new System::Windows::Forms::TextBox();
			this->label143 = new System::Windows::Forms::Label();
			this->textBox138 = new System::Windows::Forms::TextBox();
			this->groupBox23 = new System::Windows::Forms::GroupBox();
			this->textBox144 = new System::Windows::Forms::TextBox();
			this->label149 = new System::Windows::Forms::Label();
			this->label150 = new System::Windows::Forms::Label();
			this->textBox145 = new System::Windows::Forms::TextBox();
			this->label151 = new System::Windows::Forms::Label();
			this->textBox146 = new System::Windows::Forms::TextBox();
			this->groupBox24 = new System::Windows::Forms::GroupBox();
			this->textBox179 = new System::Windows::Forms::TextBox();
			this->label184 = new System::Windows::Forms::Label();
			this->textBox178 = new System::Windows::Forms::TextBox();
			this->label183 = new System::Windows::Forms::Label();
			this->textBox177 = new System::Windows::Forms::TextBox();
			this->label182 = new System::Windows::Forms::Label();
			this->textBox176 = new System::Windows::Forms::TextBox();
			this->label181 = new System::Windows::Forms::Label();
			this->textBox175 = new System::Windows::Forms::TextBox();
			this->label180 = new System::Windows::Forms::Label();
			this->textBox170 = new System::Windows::Forms::TextBox();
			this->label175 = new System::Windows::Forms::Label();
			this->textBox171 = new System::Windows::Forms::TextBox();
			this->label176 = new System::Windows::Forms::Label();
			this->textBox167 = new System::Windows::Forms::TextBox();
			this->label172 = new System::Windows::Forms::Label();
			this->textBox168 = new System::Windows::Forms::TextBox();
			this->label173 = new System::Windows::Forms::Label();
			this->textBox169 = new System::Windows::Forms::TextBox();
			this->label174 = new System::Windows::Forms::Label();
			this->textBox165 = new System::Windows::Forms::TextBox();
			this->label170 = new System::Windows::Forms::Label();
			this->textBox166 = new System::Windows::Forms::TextBox();
			this->label171 = new System::Windows::Forms::Label();
			this->textBox164 = new System::Windows::Forms::TextBox();
			this->label169 = new System::Windows::Forms::Label();
			this->textBox160 = new System::Windows::Forms::TextBox();
			this->label165 = new System::Windows::Forms::Label();
			this->textBox151 = new System::Windows::Forms::TextBox();
			this->label156 = new System::Windows::Forms::Label();
			this->textBox150 = new System::Windows::Forms::TextBox();
			this->label155 = new System::Windows::Forms::Label();
			this->textBox149 = new System::Windows::Forms::TextBox();
			this->label154 = new System::Windows::Forms::Label();
			this->textBox148 = new System::Windows::Forms::TextBox();
			this->label153 = new System::Windows::Forms::Label();
			this->textBox147 = new System::Windows::Forms::TextBox();
			this->label152 = new System::Windows::Forms::Label();
			this->groupBox25 = new System::Windows::Forms::GroupBox();
			this->textBox154 = new System::Windows::Forms::TextBox();
			this->label159 = new System::Windows::Forms::Label();
			this->label160 = new System::Windows::Forms::Label();
			this->textBox155 = new System::Windows::Forms::TextBox();
			this->label161 = new System::Windows::Forms::Label();
			this->textBox156 = new System::Windows::Forms::TextBox();
			this->label162 = new System::Windows::Forms::Label();
			this->textBox157 = new System::Windows::Forms::TextBox();
			this->groupBox26 = new System::Windows::Forms::GroupBox();
			this->textBox161 = new System::Windows::Forms::TextBox();
			this->label166 = new System::Windows::Forms::Label();
			this->label167 = new System::Windows::Forms::Label();
			this->textBox162 = new System::Windows::Forms::TextBox();
			this->label168 = new System::Windows::Forms::Label();
			this->textBox163 = new System::Windows::Forms::TextBox();
			this->groupBox27 = new System::Windows::Forms::GroupBox();
			this->textBox172 = new System::Windows::Forms::TextBox();
			this->label177 = new System::Windows::Forms::Label();
			this->label178 = new System::Windows::Forms::Label();
			this->textBox173 = new System::Windows::Forms::TextBox();
			this->label179 = new System::Windows::Forms::Label();
			this->textBox174 = new System::Windows::Forms::TextBox();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->groupBox7->SuspendLayout();
			this->groupBox8->SuspendLayout();
			this->groupBox9->SuspendLayout();
			this->groupBox10->SuspendLayout();
			this->groupBox12->SuspendLayout();
			this->groupBox13->SuspendLayout();
			this->groupBox14->SuspendLayout();
			this->groupBox15->SuspendLayout();
			this->groupBox16->SuspendLayout();
			this->groupBox17->SuspendLayout();
			this->groupBox18->SuspendLayout();
			this->groupBox19->SuspendLayout();
			this->groupBox21->SuspendLayout();
			this->groupBox20->SuspendLayout();
			this->groupBox11->SuspendLayout();
			this->groupBox22->SuspendLayout();
			this->groupBox23->SuspendLayout();
			this->groupBox24->SuspendLayout();
			this->groupBox25->SuspendLayout();
			this->groupBox26->SuspendLayout();
			this->groupBox27->SuspendLayout();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 8);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(64, 24);
			this->label1->TabIndex = 5;
			this->label1->Text = S"CMC LIST";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// cmcListID
			// 
			this->cmcListID->Enabled = false;
			this->cmcListID->Location = System::Drawing::Point(72, 8);
			this->cmcListID->Name = S"cmcListID";
			this->cmcListID->Size = System::Drawing::Size(112, 20);
			this->cmcListID->TabIndex = 102;
			this->cmcListID->Text = S"NO SYNC";
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 16);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(24, 24);
			this->label3->TabIndex = 105;
			this->label3->Text = S"Rx";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(8, 40);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(24, 24);
			this->label4->TabIndex = 106;
			this->label4->Text = S"Ry";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(8, 64);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(24, 24);
			this->label5->TabIndex = 107;
			this->label5->Text = S"Rz";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(8, 136);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(24, 24);
			this->label6->TabIndex = 110;
			this->label6->Text = S"Vz";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label7
			// 
			this->label7->Location = System::Drawing::Point(8, 112);
			this->label7->Name = S"label7";
			this->label7->Size = System::Drawing::Size(24, 24);
			this->label7->TabIndex = 109;
			this->label7->Text = S"Vy";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label8
			// 
			this->label8->Location = System::Drawing::Point(8, 88);
			this->label8->Name = S"label8";
			this->label8->Size = System::Drawing::Size(24, 24);
			this->label8->TabIndex = 108;
			this->label8->Text = S"Vx";
			this->label8->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label9
			// 
			this->label9->Location = System::Drawing::Point(8, 160);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(24, 24);
			this->label9->TabIndex = 111;
			this->label9->Text = S"Tsv";
			this->label9->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox1
			// 
			this->textBox1->Enabled = false;
			this->textBox1->Location = System::Drawing::Point(32, 16);
			this->textBox1->Name = S"textBox1";
			this->textBox1->Size = System::Drawing::Size(72, 20);
			this->textBox1->TabIndex = 112;
			this->textBox1->Text = S"00000-00000";
			// 
			// textBox2
			// 
			this->textBox2->Enabled = false;
			this->textBox2->Location = System::Drawing::Point(32, 40);
			this->textBox2->Name = S"textBox2";
			this->textBox2->Size = System::Drawing::Size(72, 20);
			this->textBox2->TabIndex = 113;
			this->textBox2->Text = S"00000-00000";
			// 
			// textBox3
			// 
			this->textBox3->Enabled = false;
			this->textBox3->Location = System::Drawing::Point(32, 64);
			this->textBox3->Name = S"textBox3";
			this->textBox3->Size = System::Drawing::Size(72, 20);
			this->textBox3->TabIndex = 114;
			this->textBox3->Text = S"00000-00000";
			// 
			// textBox4
			// 
			this->textBox4->Enabled = false;
			this->textBox4->Location = System::Drawing::Point(32, 88);
			this->textBox4->Name = S"textBox4";
			this->textBox4->Size = System::Drawing::Size(72, 20);
			this->textBox4->TabIndex = 115;
			this->textBox4->Text = S"00000-00000";
			// 
			// textBox5
			// 
			this->textBox5->Enabled = false;
			this->textBox5->Location = System::Drawing::Point(32, 112);
			this->textBox5->Name = S"textBox5";
			this->textBox5->Size = System::Drawing::Size(72, 20);
			this->textBox5->TabIndex = 116;
			this->textBox5->Text = S"00000-00000";
			// 
			// textBox6
			// 
			this->textBox6->Enabled = false;
			this->textBox6->Location = System::Drawing::Point(32, 136);
			this->textBox6->Name = S"textBox6";
			this->textBox6->Size = System::Drawing::Size(72, 20);
			this->textBox6->TabIndex = 117;
			this->textBox6->Text = S"00000-00000";
			// 
			// textBox7
			// 
			this->textBox7->Enabled = false;
			this->textBox7->Location = System::Drawing::Point(32, 160);
			this->textBox7->Name = S"textBox7";
			this->textBox7->Size = System::Drawing::Size(72, 20);
			this->textBox7->TabIndex = 118;
			this->textBox7->Text = S"00000-00000";
			// 
			// label10
			// 
			this->label10->Location = System::Drawing::Point(8, 16);
			this->label10->Name = S"label10";
			this->label10->Size = System::Drawing::Size(48, 24);
			this->label10->TabIndex = 119;
			this->label10->Text = S"X";
			this->label10->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label11
			// 
			this->label11->Location = System::Drawing::Point(8, 40);
			this->label11->Name = S"label11";
			this->label11->Size = System::Drawing::Size(48, 24);
			this->label11->TabIndex = 120;
			this->label11->Text = S"Y";
			this->label11->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label12
			// 
			this->label12->Location = System::Drawing::Point(8, 64);
			this->label12->Name = S"label12";
			this->label12->Size = System::Drawing::Size(48, 24);
			this->label12->TabIndex = 121;
			this->label12->Text = S"Z";
			this->label12->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox8
			// 
			this->textBox8->Enabled = false;
			this->textBox8->Location = System::Drawing::Point(56, 64);
			this->textBox8->Name = S"textBox8";
			this->textBox8->Size = System::Drawing::Size(72, 20);
			this->textBox8->TabIndex = 124;
			this->textBox8->Text = S"00000-00000";
			// 
			// textBox9
			// 
			this->textBox9->Enabled = false;
			this->textBox9->Location = System::Drawing::Point(56, 40);
			this->textBox9->Name = S"textBox9";
			this->textBox9->Size = System::Drawing::Size(72, 20);
			this->textBox9->TabIndex = 123;
			this->textBox9->Text = S"00000-00000";
			// 
			// textBox10
			// 
			this->textBox10->Enabled = false;
			this->textBox10->Location = System::Drawing::Point(56, 16);
			this->textBox10->Name = S"textBox10";
			this->textBox10->Size = System::Drawing::Size(72, 20);
			this->textBox10->TabIndex = 122;
			this->textBox10->Text = S"00000-00000";
			// 
			// label13
			// 
			this->label13->Location = System::Drawing::Point(8, 88);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(48, 24);
			this->label13->TabIndex = 125;
			this->label13->Text = S"SHAFT";
			this->label13->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label14
			// 
			this->label14->Location = System::Drawing::Point(8, 112);
			this->label14->Name = S"label14";
			this->label14->Size = System::Drawing::Size(48, 24);
			this->label14->TabIndex = 126;
			this->label14->Text = S"TRUN";
			this->label14->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox11
			// 
			this->textBox11->Enabled = false;
			this->textBox11->Location = System::Drawing::Point(56, 112);
			this->textBox11->Name = S"textBox11";
			this->textBox11->Size = System::Drawing::Size(72, 20);
			this->textBox11->TabIndex = 128;
			this->textBox11->Text = S"00000-00000";
			// 
			// textBox12
			// 
			this->textBox12->Enabled = false;
			this->textBox12->Location = System::Drawing::Point(56, 88);
			this->textBox12->Name = S"textBox12";
			this->textBox12->Size = System::Drawing::Size(72, 20);
			this->textBox12->TabIndex = 127;
			this->textBox12->Text = S"00000-00000";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->textBox1);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->textBox2);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->textBox3);
			this->groupBox1->Controls->Add(this->label8);
			this->groupBox1->Controls->Add(this->textBox4);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->textBox5);
			this->groupBox1->Controls->Add(this->label7);
			this->groupBox1->Controls->Add(this->textBox6);
			this->groupBox1->Controls->Add(this->label9);
			this->groupBox1->Controls->Add(this->textBox7);
			this->groupBox1->Location = System::Drawing::Point(8, 32);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(112, 192);
			this->groupBox1->TabIndex = 129;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"CM SV";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->textBox11);
			this->groupBox2->Controls->Add(this->textBox12);
			this->groupBox2->Controls->Add(this->label10);
			this->groupBox2->Controls->Add(this->label11);
			this->groupBox2->Controls->Add(this->textBox9);
			this->groupBox2->Controls->Add(this->label12);
			this->groupBox2->Controls->Add(this->textBox10);
			this->groupBox2->Controls->Add(this->textBox8);
			this->groupBox2->Controls->Add(this->label13);
			this->groupBox2->Controls->Add(this->label14);
			this->groupBox2->Location = System::Drawing::Point(8, 264);
			this->groupBox2->Name = S"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(136, 144);
			this->groupBox2->TabIndex = 130;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = S"CDU ANGLES";
			// 
			// label15
			// 
			this->label15->Location = System::Drawing::Point(8, 16);
			this->label15->Name = S"label15";
			this->label15->Size = System::Drawing::Size(16, 24);
			this->label15->TabIndex = 131;
			this->label15->Text = S"X";
			this->label15->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label16
			// 
			this->label16->Location = System::Drawing::Point(8, 40);
			this->label16->Name = S"label16";
			this->label16->Size = System::Drawing::Size(16, 24);
			this->label16->TabIndex = 132;
			this->label16->Text = S"Y";
			this->label16->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox13
			// 
			this->textBox13->Enabled = false;
			this->textBox13->Location = System::Drawing::Point(32, 40);
			this->textBox13->Name = S"textBox13";
			this->textBox13->Size = System::Drawing::Size(72, 20);
			this->textBox13->TabIndex = 135;
			this->textBox13->Text = S"00000-00000";
			// 
			// label17
			// 
			this->label17->Location = System::Drawing::Point(8, 64);
			this->label17->Name = S"label17";
			this->label17->Size = System::Drawing::Size(16, 24);
			this->label17->TabIndex = 133;
			this->label17->Text = S"Z";
			this->label17->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox14
			// 
			this->textBox14->Enabled = false;
			this->textBox14->Location = System::Drawing::Point(32, 16);
			this->textBox14->Name = S"textBox14";
			this->textBox14->Size = System::Drawing::Size(72, 20);
			this->textBox14->TabIndex = 134;
			this->textBox14->Text = S"00000-00000";
			// 
			// textBox15
			// 
			this->textBox15->Enabled = false;
			this->textBox15->Location = System::Drawing::Point(32, 64);
			this->textBox15->Name = S"textBox15";
			this->textBox15->Size = System::Drawing::Size(72, 20);
			this->textBox15->TabIndex = 136;
			this->textBox15->Text = S"00000-00000";
			// 
			// groupBox3
			// 
			this->groupBox3->Controls->Add(this->label15);
			this->groupBox3->Controls->Add(this->label16);
			this->groupBox3->Controls->Add(this->textBox13);
			this->groupBox3->Controls->Add(this->label17);
			this->groupBox3->Controls->Add(this->textBox14);
			this->groupBox3->Controls->Add(this->textBox15);
			this->groupBox3->Location = System::Drawing::Point(112, 504);
			this->groupBox3->Name = S"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(120, 96);
			this->groupBox3->TabIndex = 137;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = S"DAP BODY RATES";
			// 
			// label18
			// 
			this->label18->Location = System::Drawing::Point(8, 16);
			this->label18->Name = S"label18";
			this->label18->Size = System::Drawing::Size(32, 24);
			this->label18->TabIndex = 138;
			this->label18->Text = S"AK";
			this->label18->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label19
			// 
			this->label19->Location = System::Drawing::Point(8, 40);
			this->label19->Name = S"label19";
			this->label19->Size = System::Drawing::Size(32, 24);
			this->label19->TabIndex = 139;
			this->label19->Text = S"AK1";
			this->label19->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox16
			// 
			this->textBox16->Enabled = false;
			this->textBox16->Location = System::Drawing::Point(40, 40);
			this->textBox16->Name = S"textBox16";
			this->textBox16->Size = System::Drawing::Size(72, 20);
			this->textBox16->TabIndex = 142;
			this->textBox16->Text = S"00000-00000";
			// 
			// label20
			// 
			this->label20->Location = System::Drawing::Point(8, 64);
			this->label20->Name = S"label20";
			this->label20->Size = System::Drawing::Size(32, 24);
			this->label20->TabIndex = 140;
			this->label20->Text = S"AK2";
			this->label20->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox17
			// 
			this->textBox17->Enabled = false;
			this->textBox17->Location = System::Drawing::Point(40, 16);
			this->textBox17->Name = S"textBox17";
			this->textBox17->Size = System::Drawing::Size(72, 20);
			this->textBox17->TabIndex = 141;
			this->textBox17->Text = S"00000-00000";
			// 
			// textBox18
			// 
			this->textBox18->Enabled = false;
			this->textBox18->Location = System::Drawing::Point(40, 64);
			this->textBox18->Name = S"textBox18";
			this->textBox18->Size = System::Drawing::Size(72, 20);
			this->textBox18->TabIndex = 143;
			this->textBox18->Text = S"00000-00000";
			// 
			// groupBox4
			// 
			this->groupBox4->Controls->Add(this->label18);
			this->groupBox4->Controls->Add(this->label19);
			this->groupBox4->Controls->Add(this->textBox16);
			this->groupBox4->Controls->Add(this->label20);
			this->groupBox4->Controls->Add(this->textBox17);
			this->groupBox4->Controls->Add(this->textBox18);
			this->groupBox4->Location = System::Drawing::Point(8, 600);
			this->groupBox4->Name = S"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(120, 96);
			this->groupBox4->TabIndex = 144;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = S"FDAI ATT ERR";
			// 
			// groupBox5
			// 
			this->groupBox5->Controls->Add(this->label110);
			this->groupBox5->Controls->Add(this->textBox108);
			this->groupBox5->Controls->Add(this->label109);
			this->groupBox5->Controls->Add(this->textBox107);
			this->groupBox5->Controls->Add(this->label105);
			this->groupBox5->Controls->Add(this->textBox103);
			this->groupBox5->Controls->Add(this->label102);
			this->groupBox5->Controls->Add(this->textBox100);
			this->groupBox5->Controls->Add(this->label89);
			this->groupBox5->Controls->Add(this->textBox87);
			this->groupBox5->Controls->Add(this->label66);
			this->groupBox5->Controls->Add(this->textBox64);
			this->groupBox5->Controls->Add(this->label63);
			this->groupBox5->Controls->Add(this->textBox61);
			this->groupBox5->Controls->Add(this->label64);
			this->groupBox5->Controls->Add(this->textBox62);
			this->groupBox5->Controls->Add(this->label65);
			this->groupBox5->Controls->Add(this->textBox63);
			this->groupBox5->Controls->Add(this->label61);
			this->groupBox5->Controls->Add(this->textBox59);
			this->groupBox5->Controls->Add(this->label62);
			this->groupBox5->Controls->Add(this->textBox60);
			this->groupBox5->Controls->Add(this->label59);
			this->groupBox5->Controls->Add(this->textBox57);
			this->groupBox5->Controls->Add(this->label60);
			this->groupBox5->Controls->Add(this->textBox58);
			this->groupBox5->Controls->Add(this->label58);
			this->groupBox5->Controls->Add(this->textBox56);
			this->groupBox5->Controls->Add(this->label57);
			this->groupBox5->Controls->Add(this->textBox55);
			this->groupBox5->Controls->Add(this->label21);
			this->groupBox5->Controls->Add(this->textBox19);
			this->groupBox5->Location = System::Drawing::Point(472, 0);
			this->groupBox5->Name = S"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(96, 408);
			this->groupBox5->TabIndex = 145;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = S"FLAGS";
			// 
			// label110
			// 
			this->label110->Location = System::Drawing::Point(8, 88);
			this->label110->Name = S"label110";
			this->label110->Size = System::Drawing::Size(40, 24);
			this->label110->TabIndex = 176;
			this->label110->Text = S"DAP2";
			this->label110->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox108
			// 
			this->textBox108->Enabled = false;
			this->textBox108->Location = System::Drawing::Point(48, 88);
			this->textBox108->Name = S"textBox108";
			this->textBox108->Size = System::Drawing::Size(40, 20);
			this->textBox108->TabIndex = 177;
			this->textBox108->Text = S"00000";
			// 
			// label109
			// 
			this->label109->Location = System::Drawing::Point(8, 64);
			this->label109->Name = S"label109";
			this->label109->Size = System::Drawing::Size(40, 24);
			this->label109->TabIndex = 174;
			this->label109->Text = S"DAP1";
			this->label109->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox107
			// 
			this->textBox107->Enabled = false;
			this->textBox107->Location = System::Drawing::Point(48, 64);
			this->textBox107->Name = S"textBox107";
			this->textBox107->Size = System::Drawing::Size(40, 20);
			this->textBox107->TabIndex = 175;
			this->textBox107->Text = S"00000";
			// 
			// label105
			// 
			this->label105->Location = System::Drawing::Point(8, 40);
			this->label105->Name = S"label105";
			this->label105->Size = System::Drawing::Size(40, 24);
			this->label105->TabIndex = 172;
			this->label105->Text = S"OPT";
			this->label105->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox103
			// 
			this->textBox103->Enabled = false;
			this->textBox103->Location = System::Drawing::Point(48, 40);
			this->textBox103->Name = S"textBox103";
			this->textBox103->Size = System::Drawing::Size(40, 20);
			this->textBox103->TabIndex = 173;
			this->textBox103->Text = S"00000";
			// 
			// label102
			// 
			this->label102->Location = System::Drawing::Point(8, 376);
			this->label102->Name = S"label102";
			this->label102->Size = System::Drawing::Size(40, 24);
			this->label102->TabIndex = 170;
			this->label102->Text = S"FW 11";
			this->label102->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox100
			// 
			this->textBox100->Enabled = false;
			this->textBox100->Location = System::Drawing::Point(48, 376);
			this->textBox100->Name = S"textBox100";
			this->textBox100->Size = System::Drawing::Size(40, 20);
			this->textBox100->TabIndex = 171;
			this->textBox100->Text = S"00000";
			// 
			// label89
			// 
			this->label89->Location = System::Drawing::Point(8, 352);
			this->label89->Name = S"label89";
			this->label89->Size = System::Drawing::Size(40, 24);
			this->label89->TabIndex = 168;
			this->label89->Text = S"FW 10";
			this->label89->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox87
			// 
			this->textBox87->Enabled = false;
			this->textBox87->Location = System::Drawing::Point(48, 352);
			this->textBox87->Name = S"textBox87";
			this->textBox87->Size = System::Drawing::Size(40, 20);
			this->textBox87->TabIndex = 169;
			this->textBox87->Text = S"00000";
			// 
			// label66
			// 
			this->label66->Location = System::Drawing::Point(8, 328);
			this->label66->Name = S"label66";
			this->label66->Size = System::Drawing::Size(40, 24);
			this->label66->TabIndex = 166;
			this->label66->Text = S"FW 9";
			this->label66->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox64
			// 
			this->textBox64->Enabled = false;
			this->textBox64->Location = System::Drawing::Point(48, 328);
			this->textBox64->Name = S"textBox64";
			this->textBox64->Size = System::Drawing::Size(40, 20);
			this->textBox64->TabIndex = 167;
			this->textBox64->Text = S"00000";
			// 
			// label63
			// 
			this->label63->Location = System::Drawing::Point(8, 304);
			this->label63->Name = S"label63";
			this->label63->Size = System::Drawing::Size(40, 24);
			this->label63->TabIndex = 164;
			this->label63->Text = S"FW 8";
			this->label63->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox61
			// 
			this->textBox61->Enabled = false;
			this->textBox61->Location = System::Drawing::Point(48, 304);
			this->textBox61->Name = S"textBox61";
			this->textBox61->Size = System::Drawing::Size(40, 20);
			this->textBox61->TabIndex = 165;
			this->textBox61->Text = S"00000";
			// 
			// label64
			// 
			this->label64->Location = System::Drawing::Point(8, 280);
			this->label64->Name = S"label64";
			this->label64->Size = System::Drawing::Size(40, 24);
			this->label64->TabIndex = 162;
			this->label64->Text = S"FW 7";
			this->label64->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox62
			// 
			this->textBox62->Enabled = false;
			this->textBox62->Location = System::Drawing::Point(48, 280);
			this->textBox62->Name = S"textBox62";
			this->textBox62->Size = System::Drawing::Size(40, 20);
			this->textBox62->TabIndex = 163;
			this->textBox62->Text = S"00000";
			// 
			// label65
			// 
			this->label65->Location = System::Drawing::Point(8, 256);
			this->label65->Name = S"label65";
			this->label65->Size = System::Drawing::Size(40, 24);
			this->label65->TabIndex = 160;
			this->label65->Text = S"FW 6";
			this->label65->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox63
			// 
			this->textBox63->Enabled = false;
			this->textBox63->Location = System::Drawing::Point(48, 256);
			this->textBox63->Name = S"textBox63";
			this->textBox63->Size = System::Drawing::Size(40, 20);
			this->textBox63->TabIndex = 161;
			this->textBox63->Text = S"00000";
			// 
			// label61
			// 
			this->label61->Location = System::Drawing::Point(8, 232);
			this->label61->Name = S"label61";
			this->label61->Size = System::Drawing::Size(40, 24);
			this->label61->TabIndex = 158;
			this->label61->Text = S"FW 5";
			this->label61->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox59
			// 
			this->textBox59->Enabled = false;
			this->textBox59->Location = System::Drawing::Point(48, 232);
			this->textBox59->Name = S"textBox59";
			this->textBox59->Size = System::Drawing::Size(40, 20);
			this->textBox59->TabIndex = 159;
			this->textBox59->Text = S"00000";
			// 
			// label62
			// 
			this->label62->Location = System::Drawing::Point(8, 208);
			this->label62->Name = S"label62";
			this->label62->Size = System::Drawing::Size(40, 24);
			this->label62->TabIndex = 156;
			this->label62->Text = S"FW 4";
			this->label62->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox60
			// 
			this->textBox60->Enabled = false;
			this->textBox60->Location = System::Drawing::Point(48, 208);
			this->textBox60->Name = S"textBox60";
			this->textBox60->Size = System::Drawing::Size(40, 20);
			this->textBox60->TabIndex = 157;
			this->textBox60->Text = S"00000";
			// 
			// label59
			// 
			this->label59->Location = System::Drawing::Point(8, 184);
			this->label59->Name = S"label59";
			this->label59->Size = System::Drawing::Size(40, 24);
			this->label59->TabIndex = 154;
			this->label59->Text = S"FW 3";
			this->label59->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox57
			// 
			this->textBox57->Enabled = false;
			this->textBox57->Location = System::Drawing::Point(48, 184);
			this->textBox57->Name = S"textBox57";
			this->textBox57->Size = System::Drawing::Size(40, 20);
			this->textBox57->TabIndex = 155;
			this->textBox57->Text = S"00000";
			// 
			// label60
			// 
			this->label60->Location = System::Drawing::Point(8, 160);
			this->label60->Name = S"label60";
			this->label60->Size = System::Drawing::Size(40, 24);
			this->label60->TabIndex = 152;
			this->label60->Text = S"FW 2";
			this->label60->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox58
			// 
			this->textBox58->Enabled = false;
			this->textBox58->Location = System::Drawing::Point(48, 160);
			this->textBox58->Name = S"textBox58";
			this->textBox58->Size = System::Drawing::Size(40, 20);
			this->textBox58->TabIndex = 153;
			this->textBox58->Text = S"00000";
			// 
			// label58
			// 
			this->label58->Location = System::Drawing::Point(8, 136);
			this->label58->Name = S"label58";
			this->label58->Size = System::Drawing::Size(40, 24);
			this->label58->TabIndex = 150;
			this->label58->Text = S"FW 1";
			this->label58->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox56
			// 
			this->textBox56->Enabled = false;
			this->textBox56->Location = System::Drawing::Point(48, 136);
			this->textBox56->Name = S"textBox56";
			this->textBox56->Size = System::Drawing::Size(40, 20);
			this->textBox56->TabIndex = 151;
			this->textBox56->Text = S"00000";
			// 
			// label57
			// 
			this->label57->Location = System::Drawing::Point(8, 112);
			this->label57->Name = S"label57";
			this->label57->Size = System::Drawing::Size(40, 24);
			this->label57->TabIndex = 148;
			this->label57->Text = S"FW 0";
			this->label57->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox55
			// 
			this->textBox55->Enabled = false;
			this->textBox55->Location = System::Drawing::Point(48, 112);
			this->textBox55->Name = S"textBox55";
			this->textBox55->Size = System::Drawing::Size(40, 20);
			this->textBox55->TabIndex = 149;
			this->textBox55->Text = S"00000";
			// 
			// label21
			// 
			this->label21->Location = System::Drawing::Point(8, 16);
			this->label21->Name = S"label21";
			this->label21->Size = System::Drawing::Size(40, 24);
			this->label21->TabIndex = 146;
			this->label21->Text = S"RCS";
			this->label21->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox19
			// 
			this->textBox19->Enabled = false;
			this->textBox19->Location = System::Drawing::Point(48, 16);
			this->textBox19->Name = S"textBox19";
			this->textBox19->Size = System::Drawing::Size(40, 20);
			this->textBox19->TabIndex = 147;
			this->textBox19->Text = S"00000";
			// 
			// label22
			// 
			this->label22->Location = System::Drawing::Point(8, 16);
			this->label22->Name = S"label22";
			this->label22->Size = System::Drawing::Size(16, 24);
			this->label22->TabIndex = 146;
			this->label22->Text = S"X";
			this->label22->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label23
			// 
			this->label23->Location = System::Drawing::Point(8, 40);
			this->label23->Name = S"label23";
			this->label23->Size = System::Drawing::Size(16, 24);
			this->label23->TabIndex = 147;
			this->label23->Text = S"Y";
			this->label23->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox20
			// 
			this->textBox20->Enabled = false;
			this->textBox20->Location = System::Drawing::Point(32, 40);
			this->textBox20->Name = S"textBox20";
			this->textBox20->Size = System::Drawing::Size(72, 20);
			this->textBox20->TabIndex = 150;
			this->textBox20->Text = S"00000-00000";
			// 
			// label24
			// 
			this->label24->Location = System::Drawing::Point(8, 64);
			this->label24->Name = S"label24";
			this->label24->Size = System::Drawing::Size(16, 24);
			this->label24->TabIndex = 148;
			this->label24->Text = S"Z";
			this->label24->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox21
			// 
			this->textBox21->Enabled = false;
			this->textBox21->Location = System::Drawing::Point(32, 16);
			this->textBox21->Name = S"textBox21";
			this->textBox21->Size = System::Drawing::Size(72, 20);
			this->textBox21->TabIndex = 149;
			this->textBox21->Text = S"00000-00000";
			// 
			// textBox22
			// 
			this->textBox22->Enabled = false;
			this->textBox22->Location = System::Drawing::Point(32, 64);
			this->textBox22->Name = S"textBox22";
			this->textBox22->Size = System::Drawing::Size(72, 20);
			this->textBox22->TabIndex = 151;
			this->textBox22->Text = S"00000-00000";
			// 
			// groupBox6
			// 
			this->groupBox6->Controls->Add(this->textBox22);
			this->groupBox6->Controls->Add(this->label22);
			this->groupBox6->Controls->Add(this->label23);
			this->groupBox6->Controls->Add(this->textBox20);
			this->groupBox6->Controls->Add(this->label24);
			this->groupBox6->Controls->Add(this->textBox21);
			this->groupBox6->Location = System::Drawing::Point(112, 408);
			this->groupBox6->Name = S"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(112, 96);
			this->groupBox6->TabIndex = 152;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = S"DAP THETA ATT";
			// 
			// label25
			// 
			this->label25->Location = System::Drawing::Point(656, 152);
			this->label25->Name = S"label25";
			this->label25->Size = System::Drawing::Size(72, 24);
			this->label25->TabIndex = 153;
			this->label25->Text = S"DELCDUx";
			this->label25->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox23
			// 
			this->textBox23->Enabled = false;
			this->textBox23->Location = System::Drawing::Point(728, 152);
			this->textBox23->Name = S"textBox23";
			this->textBox23->Size = System::Drawing::Size(40, 20);
			this->textBox23->TabIndex = 154;
			this->textBox23->Text = S"00000";
			// 
			// label26
			// 
			this->label26->Location = System::Drawing::Point(8, 16);
			this->label26->Name = S"label26";
			this->label26->Size = System::Drawing::Size(24, 24);
			this->label26->TabIndex = 155;
			this->label26->Text = S"TIG";
			this->label26->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox24
			// 
			this->textBox24->Enabled = false;
			this->textBox24->Location = System::Drawing::Point(64, 16);
			this->textBox24->Name = S"textBox24";
			this->textBox24->Size = System::Drawing::Size(72, 20);
			this->textBox24->TabIndex = 156;
			this->textBox24->Text = S"00000-00000";
			// 
			// label27
			// 
			this->label27->Location = System::Drawing::Point(8, 184);
			this->label27->Name = S"label27";
			this->label27->Size = System::Drawing::Size(48, 24);
			this->label27->TabIndex = 159;
			this->label27->Text = S"BESTJ";
			this->label27->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox25
			// 
			this->textBox25->Enabled = false;
			this->textBox25->Location = System::Drawing::Point(56, 184);
			this->textBox25->Name = S"textBox25";
			this->textBox25->Size = System::Drawing::Size(72, 20);
			this->textBox25->TabIndex = 160;
			this->textBox25->Text = S"00000-00000";
			// 
			// label28
			// 
			this->label28->Location = System::Drawing::Point(8, 184);
			this->label28->Name = S"label28";
			this->label28->Size = System::Drawing::Size(48, 24);
			this->label28->TabIndex = 157;
			this->label28->Text = S"BESTI";
			this->label28->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox26
			// 
			this->textBox26->Enabled = false;
			this->textBox26->Location = System::Drawing::Point(56, 184);
			this->textBox26->Name = S"textBox26";
			this->textBox26->Size = System::Drawing::Size(72, 20);
			this->textBox26->TabIndex = 158;
			this->textBox26->Text = S"00000-00000";
			// 
			// groupBox7
			// 
			this->groupBox7->Controls->Add(this->textBox28);
			this->groupBox7->Controls->Add(this->label30);
			this->groupBox7->Controls->Add(this->textBox27);
			this->groupBox7->Controls->Add(this->label29);
			this->groupBox7->Controls->Add(this->textBox33);
			this->groupBox7->Controls->Add(this->textBox32);
			this->groupBox7->Controls->Add(this->label33);
			this->groupBox7->Controls->Add(this->textBox31);
			this->groupBox7->Controls->Add(this->textBox29);
			this->groupBox7->Controls->Add(this->textBox30);
			this->groupBox7->Controls->Add(this->label31);
			this->groupBox7->Controls->Add(this->label32);
			this->groupBox7->Controls->Add(this->label35);
			this->groupBox7->Controls->Add(this->label34);
			this->groupBox7->Controls->Add(this->label28);
			this->groupBox7->Controls->Add(this->textBox26);
			this->groupBox7->Location = System::Drawing::Point(232, 480);
			this->groupBox7->Name = S"groupBox7";
			this->groupBox7->Size = System::Drawing::Size(136, 216);
			this->groupBox7->TabIndex = 161;
			this->groupBox7->TabStop = false;
			this->groupBox7->Text = S"1ST MARK DATA";
			// 
			// textBox28
			// 
			this->textBox28->Enabled = false;
			this->textBox28->Location = System::Drawing::Point(56, 40);
			this->textBox28->Name = S"textBox28";
			this->textBox28->Size = System::Drawing::Size(72, 20);
			this->textBox28->TabIndex = 170;
			this->textBox28->Text = S"00000-00000";
			// 
			// label30
			// 
			this->label30->Location = System::Drawing::Point(8, 40);
			this->label30->Name = S"label30";
			this->label30->Size = System::Drawing::Size(48, 24);
			this->label30->TabIndex = 163;
			this->label30->Text = S"TIME1";
			this->label30->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox27
			// 
			this->textBox27->Enabled = false;
			this->textBox27->Location = System::Drawing::Point(56, 16);
			this->textBox27->Name = S"textBox27";
			this->textBox27->Size = System::Drawing::Size(72, 20);
			this->textBox27->TabIndex = 169;
			this->textBox27->Text = S"00000-00000";
			// 
			// label29
			// 
			this->label29->Location = System::Drawing::Point(8, 16);
			this->label29->Name = S"label29";
			this->label29->Size = System::Drawing::Size(48, 24);
			this->label29->TabIndex = 162;
			this->label29->Text = S"TIME2";
			this->label29->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox33
			// 
			this->textBox33->Enabled = false;
			this->textBox33->Location = System::Drawing::Point(56, 112);
			this->textBox33->Name = S"textBox33";
			this->textBox33->Size = System::Drawing::Size(72, 20);
			this->textBox33->TabIndex = 167;
			this->textBox33->Text = S"00000-00000";
			// 
			// textBox32
			// 
			this->textBox32->Enabled = false;
			this->textBox32->Location = System::Drawing::Point(56, 64);
			this->textBox32->Name = S"textBox32";
			this->textBox32->Size = System::Drawing::Size(72, 20);
			this->textBox32->TabIndex = 165;
			this->textBox32->Text = S"00000-00000";
			// 
			// label33
			// 
			this->label33->Location = System::Drawing::Point(8, 112);
			this->label33->Name = S"label33";
			this->label33->Size = System::Drawing::Size(48, 24);
			this->label33->TabIndex = 164;
			this->label33->Text = S"Z";
			this->label33->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox31
			// 
			this->textBox31->Enabled = false;
			this->textBox31->Location = System::Drawing::Point(56, 88);
			this->textBox31->Name = S"textBox31";
			this->textBox31->Size = System::Drawing::Size(72, 20);
			this->textBox31->TabIndex = 166;
			this->textBox31->Text = S"00000-00000";
			// 
			// textBox29
			// 
			this->textBox29->Enabled = false;
			this->textBox29->Location = System::Drawing::Point(56, 160);
			this->textBox29->Name = S"textBox29";
			this->textBox29->Size = System::Drawing::Size(72, 20);
			this->textBox29->TabIndex = 171;
			this->textBox29->Text = S"00000-00000";
			// 
			// textBox30
			// 
			this->textBox30->Enabled = false;
			this->textBox30->Location = System::Drawing::Point(56, 136);
			this->textBox30->Name = S"textBox30";
			this->textBox30->Size = System::Drawing::Size(72, 20);
			this->textBox30->TabIndex = 170;
			this->textBox30->Text = S"00000-00000";
			// 
			// label31
			// 
			this->label31->Location = System::Drawing::Point(8, 64);
			this->label31->Name = S"label31";
			this->label31->Size = System::Drawing::Size(48, 24);
			this->label31->TabIndex = 162;
			this->label31->Text = S"X";
			this->label31->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label32
			// 
			this->label32->Location = System::Drawing::Point(8, 88);
			this->label32->Name = S"label32";
			this->label32->Size = System::Drawing::Size(48, 24);
			this->label32->TabIndex = 163;
			this->label32->Text = S"Y";
			this->label32->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label35
			// 
			this->label35->Location = System::Drawing::Point(8, 160);
			this->label35->Name = S"label35";
			this->label35->Size = System::Drawing::Size(48, 24);
			this->label35->TabIndex = 169;
			this->label35->Text = S"TRUN";
			this->label35->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label34
			// 
			this->label34->Location = System::Drawing::Point(8, 136);
			this->label34->Name = S"label34";
			this->label34->Size = System::Drawing::Size(48, 24);
			this->label34->TabIndex = 168;
			this->label34->Text = S"SHAFT";
			this->label34->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label36
			// 
			this->label36->Location = System::Drawing::Point(656, 224);
			this->label36->Name = S"label36";
			this->label36->Size = System::Drawing::Size(32, 24);
			this->label36->TabIndex = 162;
			this->label36->Text = S"BDT";
			this->label36->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox34
			// 
			this->textBox34->Enabled = false;
			this->textBox34->Location = System::Drawing::Point(728, 224);
			this->textBox34->Name = S"textBox34";
			this->textBox34->Size = System::Drawing::Size(40, 20);
			this->textBox34->TabIndex = 163;
			this->textBox34->Text = S"00000";
			// 
			// groupBox8
			// 
			this->groupBox8->Controls->Add(this->textBox35);
			this->groupBox8->Controls->Add(this->label37);
			this->groupBox8->Controls->Add(this->textBox36);
			this->groupBox8->Controls->Add(this->label38);
			this->groupBox8->Controls->Add(this->textBox37);
			this->groupBox8->Controls->Add(this->textBox38);
			this->groupBox8->Controls->Add(this->label39);
			this->groupBox8->Controls->Add(this->textBox39);
			this->groupBox8->Controls->Add(this->textBox40);
			this->groupBox8->Controls->Add(this->textBox41);
			this->groupBox8->Controls->Add(this->label40);
			this->groupBox8->Controls->Add(this->label41);
			this->groupBox8->Controls->Add(this->label42);
			this->groupBox8->Controls->Add(this->label43);
			this->groupBox8->Controls->Add(this->label27);
			this->groupBox8->Controls->Add(this->textBox25);
			this->groupBox8->Location = System::Drawing::Point(368, 480);
			this->groupBox8->Name = S"groupBox8";
			this->groupBox8->Size = System::Drawing::Size(136, 216);
			this->groupBox8->TabIndex = 164;
			this->groupBox8->TabStop = false;
			this->groupBox8->Text = S"2ND MARK DATA";
			// 
			// textBox35
			// 
			this->textBox35->Enabled = false;
			this->textBox35->Location = System::Drawing::Point(56, 40);
			this->textBox35->Name = S"textBox35";
			this->textBox35->Size = System::Drawing::Size(72, 20);
			this->textBox35->TabIndex = 170;
			this->textBox35->Text = S"00000-00000";
			// 
			// label37
			// 
			this->label37->Location = System::Drawing::Point(8, 40);
			this->label37->Name = S"label37";
			this->label37->Size = System::Drawing::Size(48, 24);
			this->label37->TabIndex = 163;
			this->label37->Text = S"TIME1";
			this->label37->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox36
			// 
			this->textBox36->Enabled = false;
			this->textBox36->Location = System::Drawing::Point(56, 16);
			this->textBox36->Name = S"textBox36";
			this->textBox36->Size = System::Drawing::Size(72, 20);
			this->textBox36->TabIndex = 169;
			this->textBox36->Text = S"00000-00000";
			// 
			// label38
			// 
			this->label38->Location = System::Drawing::Point(8, 16);
			this->label38->Name = S"label38";
			this->label38->Size = System::Drawing::Size(48, 24);
			this->label38->TabIndex = 162;
			this->label38->Text = S"TIME2";
			this->label38->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox37
			// 
			this->textBox37->Enabled = false;
			this->textBox37->Location = System::Drawing::Point(56, 112);
			this->textBox37->Name = S"textBox37";
			this->textBox37->Size = System::Drawing::Size(72, 20);
			this->textBox37->TabIndex = 167;
			this->textBox37->Text = S"00000-00000";
			// 
			// textBox38
			// 
			this->textBox38->Enabled = false;
			this->textBox38->Location = System::Drawing::Point(56, 64);
			this->textBox38->Name = S"textBox38";
			this->textBox38->Size = System::Drawing::Size(72, 20);
			this->textBox38->TabIndex = 165;
			this->textBox38->Text = S"00000-00000";
			// 
			// label39
			// 
			this->label39->Location = System::Drawing::Point(8, 112);
			this->label39->Name = S"label39";
			this->label39->Size = System::Drawing::Size(48, 24);
			this->label39->TabIndex = 164;
			this->label39->Text = S"Z";
			this->label39->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox39
			// 
			this->textBox39->Enabled = false;
			this->textBox39->Location = System::Drawing::Point(56, 88);
			this->textBox39->Name = S"textBox39";
			this->textBox39->Size = System::Drawing::Size(72, 20);
			this->textBox39->TabIndex = 166;
			this->textBox39->Text = S"00000-00000";
			// 
			// textBox40
			// 
			this->textBox40->Enabled = false;
			this->textBox40->Location = System::Drawing::Point(56, 160);
			this->textBox40->Name = S"textBox40";
			this->textBox40->Size = System::Drawing::Size(72, 20);
			this->textBox40->TabIndex = 171;
			this->textBox40->Text = S"00000-00000";
			// 
			// textBox41
			// 
			this->textBox41->Enabled = false;
			this->textBox41->Location = System::Drawing::Point(56, 136);
			this->textBox41->Name = S"textBox41";
			this->textBox41->Size = System::Drawing::Size(72, 20);
			this->textBox41->TabIndex = 170;
			this->textBox41->Text = S"00000-00000";
			// 
			// label40
			// 
			this->label40->Location = System::Drawing::Point(8, 64);
			this->label40->Name = S"label40";
			this->label40->Size = System::Drawing::Size(48, 24);
			this->label40->TabIndex = 162;
			this->label40->Text = S"X";
			this->label40->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label41
			// 
			this->label41->Location = System::Drawing::Point(8, 88);
			this->label41->Name = S"label41";
			this->label41->Size = System::Drawing::Size(48, 24);
			this->label41->TabIndex = 163;
			this->label41->Text = S"Y";
			this->label41->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label42
			// 
			this->label42->Location = System::Drawing::Point(8, 160);
			this->label42->Name = S"label42";
			this->label42->Size = System::Drawing::Size(48, 24);
			this->label42->TabIndex = 169;
			this->label42->Text = S"TRUN";
			this->label42->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label43
			// 
			this->label43->Location = System::Drawing::Point(8, 136);
			this->label43->Name = S"label43";
			this->label43->Size = System::Drawing::Size(48, 24);
			this->label43->TabIndex = 168;
			this->label43->Text = S"SHAFT";
			this->label43->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label44
			// 
			this->label44->Location = System::Drawing::Point(656, 176);
			this->label44->Name = S"label44";
			this->label44->Size = System::Drawing::Size(56, 24);
			this->label44->TabIndex = 165;
			this->label44->Text = S"UNITV+2";
			this->label44->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox42
			// 
			this->textBox42->Enabled = false;
			this->textBox42->Location = System::Drawing::Point(728, 176);
			this->textBox42->Name = S"textBox42";
			this->textBox42->Size = System::Drawing::Size(40, 20);
			this->textBox42->TabIndex = 166;
			this->textBox42->Text = S"00000";
			// 
			// label45
			// 
			this->label45->Location = System::Drawing::Point(776, 80);
			this->label45->Name = S"label45";
			this->label45->Size = System::Drawing::Size(72, 24);
			this->label45->TabIndex = 169;
			this->label45->Text = S"HPER";
			this->label45->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox43
			// 
			this->textBox43->Enabled = false;
			this->textBox43->Location = System::Drawing::Point(848, 80);
			this->textBox43->Name = S"textBox43";
			this->textBox43->Size = System::Drawing::Size(72, 20);
			this->textBox43->TabIndex = 170;
			this->textBox43->Text = S"00000-00000";
			// 
			// label46
			// 
			this->label46->Location = System::Drawing::Point(776, 56);
			this->label46->Name = S"label46";
			this->label46->Size = System::Drawing::Size(72, 24);
			this->label46->TabIndex = 167;
			this->label46->Text = S"HAPO";
			this->label46->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox44
			// 
			this->textBox44->Enabled = false;
			this->textBox44->Location = System::Drawing::Point(848, 56);
			this->textBox44->Name = S"textBox44";
			this->textBox44->Size = System::Drawing::Size(72, 20);
			this->textBox44->TabIndex = 168;
			this->textBox44->Text = S"00000-00000";
			// 
			// label47
			// 
			this->label47->Location = System::Drawing::Point(776, 104);
			this->label47->Name = S"label47";
			this->label47->Size = System::Drawing::Size(72, 24);
			this->label47->TabIndex = 171;
			this->label47->Text = S"RSP-RREC";
			this->label47->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox45
			// 
			this->textBox45->Enabled = false;
			this->textBox45->Location = System::Drawing::Point(848, 104);
			this->textBox45->Name = S"textBox45";
			this->textBox45->Size = System::Drawing::Size(72, 20);
			this->textBox45->TabIndex = 172;
			this->textBox45->Text = S"00000-00000";
			// 
			// groupBox9
			// 
			this->groupBox9->Controls->Add(this->textBox46);
			this->groupBox9->Controls->Add(this->label48);
			this->groupBox9->Controls->Add(this->label49);
			this->groupBox9->Controls->Add(this->textBox47);
			this->groupBox9->Controls->Add(this->label50);
			this->groupBox9->Controls->Add(this->textBox48);
			this->groupBox9->Location = System::Drawing::Point(504, 600);
			this->groupBox9->Name = S"groupBox9";
			this->groupBox9->Size = System::Drawing::Size(104, 96);
			this->groupBox9->TabIndex = 173;
			this->groupBox9->TabStop = false;
			this->groupBox9->Text = S"VGTIG";
			// 
			// textBox46
			// 
			this->textBox46->Enabled = false;
			this->textBox46->Location = System::Drawing::Point(24, 64);
			this->textBox46->Name = S"textBox46";
			this->textBox46->Size = System::Drawing::Size(72, 20);
			this->textBox46->TabIndex = 151;
			this->textBox46->Text = S"00000-00000";
			// 
			// label48
			// 
			this->label48->Location = System::Drawing::Point(8, 16);
			this->label48->Name = S"label48";
			this->label48->Size = System::Drawing::Size(16, 24);
			this->label48->TabIndex = 146;
			this->label48->Text = S"X";
			this->label48->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label49
			// 
			this->label49->Location = System::Drawing::Point(8, 40);
			this->label49->Name = S"label49";
			this->label49->Size = System::Drawing::Size(16, 24);
			this->label49->TabIndex = 147;
			this->label49->Text = S"Y";
			this->label49->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox47
			// 
			this->textBox47->Enabled = false;
			this->textBox47->Location = System::Drawing::Point(24, 40);
			this->textBox47->Name = S"textBox47";
			this->textBox47->Size = System::Drawing::Size(72, 20);
			this->textBox47->TabIndex = 150;
			this->textBox47->Text = S"00000-00000";
			// 
			// label50
			// 
			this->label50->Location = System::Drawing::Point(8, 64);
			this->label50->Name = S"label50";
			this->label50->Size = System::Drawing::Size(16, 24);
			this->label50->TabIndex = 148;
			this->label50->Text = S"Z";
			this->label50->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox48
			// 
			this->textBox48->Enabled = false;
			this->textBox48->Location = System::Drawing::Point(24, 16);
			this->textBox48->Name = S"textBox48";
			this->textBox48->Size = System::Drawing::Size(72, 20);
			this->textBox48->TabIndex = 149;
			this->textBox48->Text = S"00000-00000";
			// 
			// groupBox10
			// 
			this->groupBox10->Controls->Add(this->label51);
			this->groupBox10->Controls->Add(this->textBox49);
			this->groupBox10->Controls->Add(this->label52);
			this->groupBox10->Controls->Add(this->textBox50);
			this->groupBox10->Controls->Add(this->label53);
			this->groupBox10->Controls->Add(this->textBox51);
			this->groupBox10->Controls->Add(this->label54);
			this->groupBox10->Controls->Add(this->textBox52);
			this->groupBox10->Controls->Add(this->label55);
			this->groupBox10->Controls->Add(this->textBox53);
			this->groupBox10->Controls->Add(this->label56);
			this->groupBox10->Controls->Add(this->textBox54);
			this->groupBox10->Location = System::Drawing::Point(120, 32);
			this->groupBox10->Name = S"groupBox10";
			this->groupBox10->Size = System::Drawing::Size(112, 168);
			this->groupBox10->TabIndex = 174;
			this->groupBox10->TabStop = false;
			this->groupBox10->Text = S"REFSMMAT";
			// 
			// label51
			// 
			this->label51->Location = System::Drawing::Point(8, 16);
			this->label51->Name = S"label51";
			this->label51->Size = System::Drawing::Size(24, 24);
			this->label51->TabIndex = 105;
			this->label51->Text = S"Rx";
			this->label51->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox49
			// 
			this->textBox49->Enabled = false;
			this->textBox49->Location = System::Drawing::Point(32, 16);
			this->textBox49->Name = S"textBox49";
			this->textBox49->Size = System::Drawing::Size(72, 20);
			this->textBox49->TabIndex = 112;
			this->textBox49->Text = S"00000-00000";
			// 
			// label52
			// 
			this->label52->Location = System::Drawing::Point(8, 40);
			this->label52->Name = S"label52";
			this->label52->Size = System::Drawing::Size(24, 24);
			this->label52->TabIndex = 106;
			this->label52->Text = S"Ry";
			this->label52->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox50
			// 
			this->textBox50->Enabled = false;
			this->textBox50->Location = System::Drawing::Point(32, 40);
			this->textBox50->Name = S"textBox50";
			this->textBox50->Size = System::Drawing::Size(72, 20);
			this->textBox50->TabIndex = 113;
			this->textBox50->Text = S"00000-00000";
			// 
			// label53
			// 
			this->label53->Location = System::Drawing::Point(8, 64);
			this->label53->Name = S"label53";
			this->label53->Size = System::Drawing::Size(24, 24);
			this->label53->TabIndex = 107;
			this->label53->Text = S"Rz";
			this->label53->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox51
			// 
			this->textBox51->Enabled = false;
			this->textBox51->Location = System::Drawing::Point(32, 64);
			this->textBox51->Name = S"textBox51";
			this->textBox51->Size = System::Drawing::Size(72, 20);
			this->textBox51->TabIndex = 114;
			this->textBox51->Text = S"00000-00000";
			// 
			// label54
			// 
			this->label54->Location = System::Drawing::Point(8, 88);
			this->label54->Name = S"label54";
			this->label54->Size = System::Drawing::Size(24, 24);
			this->label54->TabIndex = 108;
			this->label54->Text = S"Vx";
			this->label54->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox52
			// 
			this->textBox52->Enabled = false;
			this->textBox52->Location = System::Drawing::Point(32, 88);
			this->textBox52->Name = S"textBox52";
			this->textBox52->Size = System::Drawing::Size(72, 20);
			this->textBox52->TabIndex = 115;
			this->textBox52->Text = S"00000-00000";
			// 
			// label55
			// 
			this->label55->Location = System::Drawing::Point(8, 136);
			this->label55->Name = S"label55";
			this->label55->Size = System::Drawing::Size(24, 24);
			this->label55->TabIndex = 110;
			this->label55->Text = S"Vz";
			this->label55->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox53
			// 
			this->textBox53->Enabled = false;
			this->textBox53->Location = System::Drawing::Point(32, 112);
			this->textBox53->Name = S"textBox53";
			this->textBox53->Size = System::Drawing::Size(72, 20);
			this->textBox53->TabIndex = 116;
			this->textBox53->Text = S"00000-00000";
			// 
			// label56
			// 
			this->label56->Location = System::Drawing::Point(8, 112);
			this->label56->Name = S"label56";
			this->label56->Size = System::Drawing::Size(24, 24);
			this->label56->TabIndex = 109;
			this->label56->Text = S"Vy";
			this->label56->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox54
			// 
			this->textBox54->Enabled = false;
			this->textBox54->Location = System::Drawing::Point(32, 136);
			this->textBox54->Name = S"textBox54";
			this->textBox54->Size = System::Drawing::Size(72, 20);
			this->textBox54->TabIndex = 117;
			this->textBox54->Text = S"00000-00000";
			// 
			// label79
			// 
			this->label79->Location = System::Drawing::Point(184, 8);
			this->label79->Name = S"label79";
			this->label79->Size = System::Drawing::Size(40, 24);
			this->label79->TabIndex = 178;
			this->label79->Text = S"TIME1";
			this->label79->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox77
			// 
			this->textBox77->Enabled = false;
			this->textBox77->Location = System::Drawing::Point(224, 8);
			this->textBox77->Name = S"textBox77";
			this->textBox77->Size = System::Drawing::Size(72, 20);
			this->textBox77->TabIndex = 179;
			this->textBox77->Text = S"00000-00000";
			// 
			// label80
			// 
			this->label80->Location = System::Drawing::Point(296, 8);
			this->label80->Name = S"label80";
			this->label80->Size = System::Drawing::Size(40, 24);
			this->label80->TabIndex = 176;
			this->label80->Text = S"TIME2";
			this->label80->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox78
			// 
			this->textBox78->Enabled = false;
			this->textBox78->Location = System::Drawing::Point(336, 8);
			this->textBox78->Name = S"textBox78";
			this->textBox78->Size = System::Drawing::Size(72, 20);
			this->textBox78->TabIndex = 177;
			this->textBox78->Text = S"00000-00000";
			// 
			// groupBox12
			// 
			this->groupBox12->Controls->Add(this->label81);
			this->groupBox12->Controls->Add(this->textBox79);
			this->groupBox12->Controls->Add(this->label82);
			this->groupBox12->Controls->Add(this->textBox80);
			this->groupBox12->Controls->Add(this->label83);
			this->groupBox12->Controls->Add(this->textBox81);
			this->groupBox12->Controls->Add(this->label84);
			this->groupBox12->Controls->Add(this->textBox82);
			this->groupBox12->Controls->Add(this->label85);
			this->groupBox12->Controls->Add(this->textBox83);
			this->groupBox12->Controls->Add(this->label86);
			this->groupBox12->Controls->Add(this->textBox84);
			this->groupBox12->Controls->Add(this->label87);
			this->groupBox12->Controls->Add(this->textBox85);
			this->groupBox12->Location = System::Drawing::Point(232, 32);
			this->groupBox12->Name = S"groupBox12";
			this->groupBox12->Size = System::Drawing::Size(112, 192);
			this->groupBox12->TabIndex = 180;
			this->groupBox12->TabStop = false;
			this->groupBox12->Text = S"OTHER SV";
			// 
			// label81
			// 
			this->label81->Location = System::Drawing::Point(8, 16);
			this->label81->Name = S"label81";
			this->label81->Size = System::Drawing::Size(24, 24);
			this->label81->TabIndex = 105;
			this->label81->Text = S"Rx";
			this->label81->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox79
			// 
			this->textBox79->Enabled = false;
			this->textBox79->Location = System::Drawing::Point(32, 16);
			this->textBox79->Name = S"textBox79";
			this->textBox79->Size = System::Drawing::Size(72, 20);
			this->textBox79->TabIndex = 112;
			this->textBox79->Text = S"00000-00000";
			// 
			// label82
			// 
			this->label82->Location = System::Drawing::Point(8, 40);
			this->label82->Name = S"label82";
			this->label82->Size = System::Drawing::Size(24, 24);
			this->label82->TabIndex = 106;
			this->label82->Text = S"Ry";
			this->label82->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox80
			// 
			this->textBox80->Enabled = false;
			this->textBox80->Location = System::Drawing::Point(32, 40);
			this->textBox80->Name = S"textBox80";
			this->textBox80->Size = System::Drawing::Size(72, 20);
			this->textBox80->TabIndex = 113;
			this->textBox80->Text = S"00000-00000";
			// 
			// label83
			// 
			this->label83->Location = System::Drawing::Point(8, 64);
			this->label83->Name = S"label83";
			this->label83->Size = System::Drawing::Size(24, 24);
			this->label83->TabIndex = 107;
			this->label83->Text = S"Rz";
			this->label83->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox81
			// 
			this->textBox81->Enabled = false;
			this->textBox81->Location = System::Drawing::Point(32, 64);
			this->textBox81->Name = S"textBox81";
			this->textBox81->Size = System::Drawing::Size(72, 20);
			this->textBox81->TabIndex = 114;
			this->textBox81->Text = S"00000-00000";
			// 
			// label84
			// 
			this->label84->Location = System::Drawing::Point(8, 88);
			this->label84->Name = S"label84";
			this->label84->Size = System::Drawing::Size(24, 24);
			this->label84->TabIndex = 108;
			this->label84->Text = S"Vx";
			this->label84->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox82
			// 
			this->textBox82->Enabled = false;
			this->textBox82->Location = System::Drawing::Point(32, 88);
			this->textBox82->Name = S"textBox82";
			this->textBox82->Size = System::Drawing::Size(72, 20);
			this->textBox82->TabIndex = 115;
			this->textBox82->Text = S"00000-00000";
			// 
			// label85
			// 
			this->label85->Location = System::Drawing::Point(8, 136);
			this->label85->Name = S"label85";
			this->label85->Size = System::Drawing::Size(24, 24);
			this->label85->TabIndex = 110;
			this->label85->Text = S"Vz";
			this->label85->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox83
			// 
			this->textBox83->Enabled = false;
			this->textBox83->Location = System::Drawing::Point(32, 112);
			this->textBox83->Name = S"textBox83";
			this->textBox83->Size = System::Drawing::Size(72, 20);
			this->textBox83->TabIndex = 116;
			this->textBox83->Text = S"00000-00000";
			// 
			// label86
			// 
			this->label86->Location = System::Drawing::Point(8, 112);
			this->label86->Name = S"label86";
			this->label86->Size = System::Drawing::Size(24, 24);
			this->label86->TabIndex = 109;
			this->label86->Text = S"Vy";
			this->label86->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox84
			// 
			this->textBox84->Enabled = false;
			this->textBox84->Location = System::Drawing::Point(32, 136);
			this->textBox84->Name = S"textBox84";
			this->textBox84->Size = System::Drawing::Size(72, 20);
			this->textBox84->TabIndex = 117;
			this->textBox84->Text = S"00000-00000";
			// 
			// label87
			// 
			this->label87->Location = System::Drawing::Point(8, 160);
			this->label87->Name = S"label87";
			this->label87->Size = System::Drawing::Size(24, 24);
			this->label87->TabIndex = 111;
			this->label87->Text = S"Tsv";
			this->label87->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox85
			// 
			this->textBox85->Enabled = false;
			this->textBox85->Location = System::Drawing::Point(32, 160);
			this->textBox85->Name = S"textBox85";
			this->textBox85->Size = System::Drawing::Size(72, 20);
			this->textBox85->TabIndex = 118;
			this->textBox85->Text = S"00000-00000";
			// 
			// label88
			// 
			this->label88->Location = System::Drawing::Point(776, 128);
			this->label88->Name = S"label88";
			this->label88->Size = System::Drawing::Size(48, 24);
			this->label88->TabIndex = 181;
			this->label88->Text = S"RSBB-Q";
			this->label88->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox86
			// 
			this->textBox86->Enabled = false;
			this->textBox86->Location = System::Drawing::Point(848, 128);
			this->textBox86->Name = S"textBox86";
			this->textBox86->Size = System::Drawing::Size(72, 20);
			this->textBox86->TabIndex = 182;
			this->textBox86->Text = S"00000-00000";
			// 
			// label90
			// 
			this->label90->Location = System::Drawing::Point(656, 56);
			this->label90->Name = S"label90";
			this->label90->Size = System::Drawing::Size(72, 24);
			this->label90->TabIndex = 187;
			this->label90->Text = S"CADRFLSH2";
			this->label90->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox88
			// 
			this->textBox88->Enabled = false;
			this->textBox88->Location = System::Drawing::Point(728, 56);
			this->textBox88->Name = S"textBox88";
			this->textBox88->Size = System::Drawing::Size(40, 20);
			this->textBox88->TabIndex = 188;
			this->textBox88->Text = S"00000";
			// 
			// label91
			// 
			this->label91->Location = System::Drawing::Point(656, 32);
			this->label91->Name = S"label91";
			this->label91->Size = System::Drawing::Size(72, 24);
			this->label91->TabIndex = 185;
			this->label91->Text = S"CADRFLSH1";
			this->label91->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox89
			// 
			this->textBox89->Enabled = false;
			this->textBox89->Location = System::Drawing::Point(728, 32);
			this->textBox89->Name = S"textBox89";
			this->textBox89->Size = System::Drawing::Size(40, 20);
			this->textBox89->TabIndex = 186;
			this->textBox89->Text = S"00000";
			// 
			// label92
			// 
			this->label92->Location = System::Drawing::Point(656, 8);
			this->label92->Name = S"label92";
			this->label92->Size = System::Drawing::Size(72, 24);
			this->label92->TabIndex = 183;
			this->label92->Text = S"CADRFLSH";
			this->label92->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox90
			// 
			this->textBox90->Enabled = false;
			this->textBox90->Location = System::Drawing::Point(728, 8);
			this->textBox90->Name = S"textBox90";
			this->textBox90->Size = System::Drawing::Size(40, 20);
			this->textBox90->TabIndex = 184;
			this->textBox90->Text = S"00000";
			// 
			// groupBox13
			// 
			this->groupBox13->Controls->Add(this->label158);
			this->groupBox13->Controls->Add(this->textBox153);
			this->groupBox13->Controls->Add(this->label93);
			this->groupBox13->Controls->Add(this->label94);
			this->groupBox13->Controls->Add(this->textBox91);
			this->groupBox13->Controls->Add(this->label95);
			this->groupBox13->Controls->Add(this->textBox92);
			this->groupBox13->Controls->Add(this->textBox93);
			this->groupBox13->Location = System::Drawing::Point(144, 264);
			this->groupBox13->Name = S"groupBox13";
			this->groupBox13->Size = System::Drawing::Size(120, 120);
			this->groupBox13->TabIndex = 189;
			this->groupBox13->TabStop = false;
			this->groupBox13->Text = S"PIPA";
			// 
			// label158
			// 
			this->label158->Location = System::Drawing::Point(8, 88);
			this->label158->Name = S"label158";
			this->label158->Size = System::Drawing::Size(32, 24);
			this->label158->TabIndex = 137;
			this->label158->Text = S"TIME";
			this->label158->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox153
			// 
			this->textBox153->Enabled = false;
			this->textBox153->Location = System::Drawing::Point(40, 88);
			this->textBox153->Name = S"textBox153";
			this->textBox153->Size = System::Drawing::Size(72, 20);
			this->textBox153->TabIndex = 138;
			this->textBox153->Text = S"00000-00000";
			// 
			// label93
			// 
			this->label93->Location = System::Drawing::Point(8, 16);
			this->label93->Name = S"label93";
			this->label93->Size = System::Drawing::Size(16, 24);
			this->label93->TabIndex = 131;
			this->label93->Text = S"X";
			this->label93->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label94
			// 
			this->label94->Location = System::Drawing::Point(8, 40);
			this->label94->Name = S"label94";
			this->label94->Size = System::Drawing::Size(16, 24);
			this->label94->TabIndex = 132;
			this->label94->Text = S"Y";
			this->label94->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox91
			// 
			this->textBox91->Enabled = false;
			this->textBox91->Location = System::Drawing::Point(40, 40);
			this->textBox91->Name = S"textBox91";
			this->textBox91->Size = System::Drawing::Size(72, 20);
			this->textBox91->TabIndex = 135;
			this->textBox91->Text = S"00000-00000";
			// 
			// label95
			// 
			this->label95->Location = System::Drawing::Point(8, 64);
			this->label95->Name = S"label95";
			this->label95->Size = System::Drawing::Size(16, 24);
			this->label95->TabIndex = 133;
			this->label95->Text = S"Z";
			this->label95->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox92
			// 
			this->textBox92->Enabled = false;
			this->textBox92->Location = System::Drawing::Point(40, 16);
			this->textBox92->Name = S"textBox92";
			this->textBox92->Size = System::Drawing::Size(72, 20);
			this->textBox92->TabIndex = 134;
			this->textBox92->Text = S"00000-00000";
			// 
			// textBox93
			// 
			this->textBox93->Enabled = false;
			this->textBox93->Location = System::Drawing::Point(40, 64);
			this->textBox93->Name = S"textBox93";
			this->textBox93->Size = System::Drawing::Size(72, 20);
			this->textBox93->TabIndex = 136;
			this->textBox93->Text = S"00000-00000";
			// 
			// groupBox14
			// 
			this->groupBox14->Controls->Add(this->textBox94);
			this->groupBox14->Controls->Add(this->label96);
			this->groupBox14->Controls->Add(this->label97);
			this->groupBox14->Controls->Add(this->textBox95);
			this->groupBox14->Controls->Add(this->label98);
			this->groupBox14->Controls->Add(this->textBox96);
			this->groupBox14->Location = System::Drawing::Point(224, 384);
			this->groupBox14->Name = S"groupBox14";
			this->groupBox14->Size = System::Drawing::Size(112, 96);
			this->groupBox14->TabIndex = 190;
			this->groupBox14->TabStop = false;
			this->groupBox14->Text = S"IMU TQ ANGLES";
			// 
			// textBox94
			// 
			this->textBox94->Enabled = false;
			this->textBox94->Location = System::Drawing::Point(32, 64);
			this->textBox94->Name = S"textBox94";
			this->textBox94->Size = System::Drawing::Size(72, 20);
			this->textBox94->TabIndex = 151;
			this->textBox94->Text = S"00000-00000";
			// 
			// label96
			// 
			this->label96->Location = System::Drawing::Point(8, 16);
			this->label96->Name = S"label96";
			this->label96->Size = System::Drawing::Size(16, 24);
			this->label96->TabIndex = 146;
			this->label96->Text = S"O";
			this->label96->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label97
			// 
			this->label97->Location = System::Drawing::Point(8, 40);
			this->label97->Name = S"label97";
			this->label97->Size = System::Drawing::Size(16, 24);
			this->label97->TabIndex = 147;
			this->label97->Text = S"M";
			this->label97->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox95
			// 
			this->textBox95->Enabled = false;
			this->textBox95->Location = System::Drawing::Point(32, 40);
			this->textBox95->Name = S"textBox95";
			this->textBox95->Size = System::Drawing::Size(72, 20);
			this->textBox95->TabIndex = 150;
			this->textBox95->Text = S"00000-00000";
			// 
			// label98
			// 
			this->label98->Location = System::Drawing::Point(8, 64);
			this->label98->Name = S"label98";
			this->label98->Size = System::Drawing::Size(16, 24);
			this->label98->TabIndex = 148;
			this->label98->Text = S"I";
			this->label98->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox96
			// 
			this->textBox96->Enabled = false;
			this->textBox96->Location = System::Drawing::Point(32, 16);
			this->textBox96->Name = S"textBox96";
			this->textBox96->Size = System::Drawing::Size(72, 20);
			this->textBox96->TabIndex = 149;
			this->textBox96->Text = S"00000-00000";
			// 
			// groupBox15
			// 
			this->groupBox15->Controls->Add(this->textBox97);
			this->groupBox15->Controls->Add(this->label99);
			this->groupBox15->Controls->Add(this->label100);
			this->groupBox15->Controls->Add(this->textBox98);
			this->groupBox15->Controls->Add(this->label101);
			this->groupBox15->Controls->Add(this->textBox99);
			this->groupBox15->Location = System::Drawing::Point(400, 224);
			this->groupBox15->Name = S"groupBox15";
			this->groupBox15->Size = System::Drawing::Size(72, 96);
			this->groupBox15->TabIndex = 191;
			this->groupBox15->TabStop = false;
			this->groupBox15->Text = S"ALARMS";
			// 
			// textBox97
			// 
			this->textBox97->Enabled = false;
			this->textBox97->Location = System::Drawing::Point(24, 64);
			this->textBox97->Name = S"textBox97";
			this->textBox97->Size = System::Drawing::Size(40, 20);
			this->textBox97->TabIndex = 151;
			this->textBox97->Text = S"00000";
			// 
			// label99
			// 
			this->label99->Location = System::Drawing::Point(8, 16);
			this->label99->Name = S"label99";
			this->label99->Size = System::Drawing::Size(16, 24);
			this->label99->TabIndex = 146;
			this->label99->Text = S"1";
			this->label99->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label100
			// 
			this->label100->Location = System::Drawing::Point(8, 40);
			this->label100->Name = S"label100";
			this->label100->Size = System::Drawing::Size(16, 24);
			this->label100->TabIndex = 147;
			this->label100->Text = S"2";
			this->label100->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox98
			// 
			this->textBox98->Enabled = false;
			this->textBox98->Location = System::Drawing::Point(24, 40);
			this->textBox98->Name = S"textBox98";
			this->textBox98->Size = System::Drawing::Size(40, 20);
			this->textBox98->TabIndex = 150;
			this->textBox98->Text = S"00000";
			// 
			// label101
			// 
			this->label101->Location = System::Drawing::Point(8, 64);
			this->label101->Name = S"label101";
			this->label101->Size = System::Drawing::Size(16, 24);
			this->label101->TabIndex = 148;
			this->label101->Text = S"3";
			this->label101->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox99
			// 
			this->textBox99->Enabled = false;
			this->textBox99->Location = System::Drawing::Point(24, 16);
			this->textBox99->Name = S"textBox99";
			this->textBox99->Size = System::Drawing::Size(40, 20);
			this->textBox99->TabIndex = 149;
			this->textBox99->Text = S"00000";
			// 
			// label103
			// 
			this->label103->Location = System::Drawing::Point(776, 8);
			this->label103->Name = S"label103";
			this->label103->Size = System::Drawing::Size(72, 24);
			this->label103->TabIndex = 194;
			this->label103->Text = S"LAUNCHAZ";
			this->label103->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox101
			// 
			this->textBox101->Enabled = false;
			this->textBox101->Location = System::Drawing::Point(848, 8);
			this->textBox101->Name = S"textBox101";
			this->textBox101->Size = System::Drawing::Size(72, 20);
			this->textBox101->TabIndex = 195;
			this->textBox101->Text = S"00000-00000";
			// 
			// label104
			// 
			this->label104->Location = System::Drawing::Point(776, 32);
			this->label104->Name = S"label104";
			this->label104->Size = System::Drawing::Size(72, 24);
			this->label104->TabIndex = 192;
			this->label104->Text = S"TEVENT";
			this->label104->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox102
			// 
			this->textBox102->Enabled = false;
			this->textBox102->Location = System::Drawing::Point(848, 32);
			this->textBox102->Name = S"textBox102";
			this->textBox102->Size = System::Drawing::Size(72, 20);
			this->textBox102->TabIndex = 193;
			this->textBox102->Text = S"00000-00000";
			// 
			// label106
			// 
			this->label106->Location = System::Drawing::Point(656, 104);
			this->label106->Name = S"label106";
			this->label106->Size = System::Drawing::Size(64, 24);
			this->label106->TabIndex = 200;
			this->label106->Text = S"CSMMASS";
			this->label106->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox104
			// 
			this->textBox104->Enabled = false;
			this->textBox104->Location = System::Drawing::Point(728, 104);
			this->textBox104->Name = S"textBox104";
			this->textBox104->Size = System::Drawing::Size(40, 20);
			this->textBox104->TabIndex = 201;
			this->textBox104->Text = S"00000";
			// 
			// label107
			// 
			this->label107->Location = System::Drawing::Point(656, 80);
			this->label107->Name = S"label107";
			this->label107->Size = System::Drawing::Size(64, 24);
			this->label107->TabIndex = 198;
			this->label107->Text = S"LEMMASS";
			this->label107->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox105
			// 
			this->textBox105->Enabled = false;
			this->textBox105->Location = System::Drawing::Point(728, 80);
			this->textBox105->Name = S"textBox105";
			this->textBox105->Size = System::Drawing::Size(40, 20);
			this->textBox105->TabIndex = 199;
			this->textBox105->Text = S"00000";
			// 
			// label108
			// 
			this->label108->Location = System::Drawing::Point(8, 232);
			this->label108->Name = S"label108";
			this->label108->Size = System::Drawing::Size(72, 24);
			this->label108->TabIndex = 196;
			this->label108->Text = S"HOLDFLAG";
			this->label108->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox106
			// 
			this->textBox106->Enabled = false;
			this->textBox106->Location = System::Drawing::Point(80, 232);
			this->textBox106->Name = S"textBox106";
			this->textBox106->Size = System::Drawing::Size(40, 20);
			this->textBox106->TabIndex = 197;
			this->textBox106->Text = S"00000";
			// 
			// groupBox16
			// 
			this->groupBox16->Controls->Add(this->textBox109);
			this->groupBox16->Controls->Add(this->label111);
			this->groupBox16->Controls->Add(this->label112);
			this->groupBox16->Controls->Add(this->textBox110);
			this->groupBox16->Controls->Add(this->label113);
			this->groupBox16->Controls->Add(this->textBox111);
			this->groupBox16->Location = System::Drawing::Point(8, 504);
			this->groupBox16->Name = S"groupBox16";
			this->groupBox16->Size = System::Drawing::Size(104, 96);
			this->groupBox16->TabIndex = 202;
			this->groupBox16->TabStop = false;
			this->groupBox16->Text = S"DAP ATT ERR";
			// 
			// textBox109
			// 
			this->textBox109->Enabled = false;
			this->textBox109->Location = System::Drawing::Point(24, 64);
			this->textBox109->Name = S"textBox109";
			this->textBox109->Size = System::Drawing::Size(72, 20);
			this->textBox109->TabIndex = 151;
			this->textBox109->Text = S"00000-00000";
			// 
			// label111
			// 
			this->label111->Location = System::Drawing::Point(8, 16);
			this->label111->Name = S"label111";
			this->label111->Size = System::Drawing::Size(16, 24);
			this->label111->TabIndex = 146;
			this->label111->Text = S"X";
			this->label111->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label112
			// 
			this->label112->Location = System::Drawing::Point(8, 40);
			this->label112->Name = S"label112";
			this->label112->Size = System::Drawing::Size(16, 24);
			this->label112->TabIndex = 147;
			this->label112->Text = S"Y";
			this->label112->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox110
			// 
			this->textBox110->Enabled = false;
			this->textBox110->Location = System::Drawing::Point(24, 40);
			this->textBox110->Name = S"textBox110";
			this->textBox110->Size = System::Drawing::Size(72, 20);
			this->textBox110->TabIndex = 150;
			this->textBox110->Text = S"00000-00000";
			// 
			// label113
			// 
			this->label113->Location = System::Drawing::Point(8, 64);
			this->label113->Name = S"label113";
			this->label113->Size = System::Drawing::Size(16, 24);
			this->label113->TabIndex = 148;
			this->label113->Text = S"Z";
			this->label113->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox111
			// 
			this->textBox111->Enabled = false;
			this->textBox111->Location = System::Drawing::Point(24, 16);
			this->textBox111->Name = S"textBox111";
			this->textBox111->Size = System::Drawing::Size(72, 20);
			this->textBox111->TabIndex = 149;
			this->textBox111->Text = S"00000-00000";
			// 
			// groupBox17
			// 
			this->groupBox17->Controls->Add(this->label114);
			this->groupBox17->Controls->Add(this->label115);
			this->groupBox17->Controls->Add(this->textBox112);
			this->groupBox17->Controls->Add(this->label116);
			this->groupBox17->Controls->Add(this->textBox113);
			this->groupBox17->Controls->Add(this->textBox114);
			this->groupBox17->Location = System::Drawing::Point(128, 600);
			this->groupBox17->Name = S"groupBox17";
			this->groupBox17->Size = System::Drawing::Size(104, 96);
			this->groupBox17->TabIndex = 203;
			this->groupBox17->TabStop = false;
			this->groupBox17->Text = S"DAP TG RATES";
			// 
			// label114
			// 
			this->label114->Location = System::Drawing::Point(8, 16);
			this->label114->Name = S"label114";
			this->label114->Size = System::Drawing::Size(16, 24);
			this->label114->TabIndex = 131;
			this->label114->Text = S"R";
			this->label114->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label115
			// 
			this->label115->Location = System::Drawing::Point(8, 40);
			this->label115->Name = S"label115";
			this->label115->Size = System::Drawing::Size(16, 24);
			this->label115->TabIndex = 132;
			this->label115->Text = S"P";
			this->label115->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox112
			// 
			this->textBox112->Enabled = false;
			this->textBox112->Location = System::Drawing::Point(24, 40);
			this->textBox112->Name = S"textBox112";
			this->textBox112->Size = System::Drawing::Size(72, 20);
			this->textBox112->TabIndex = 135;
			this->textBox112->Text = S"00000-00000";
			// 
			// label116
			// 
			this->label116->Location = System::Drawing::Point(8, 64);
			this->label116->Name = S"label116";
			this->label116->Size = System::Drawing::Size(16, 24);
			this->label116->TabIndex = 133;
			this->label116->Text = S"Y";
			this->label116->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox113
			// 
			this->textBox113->Enabled = false;
			this->textBox113->Location = System::Drawing::Point(24, 16);
			this->textBox113->Name = S"textBox113";
			this->textBox113->Size = System::Drawing::Size(72, 20);
			this->textBox113->TabIndex = 134;
			this->textBox113->Text = S"00000-00000";
			// 
			// textBox114
			// 
			this->textBox114->Enabled = false;
			this->textBox114->Location = System::Drawing::Point(24, 64);
			this->textBox114->Name = S"textBox114";
			this->textBox114->Size = System::Drawing::Size(72, 20);
			this->textBox114->TabIndex = 136;
			this->textBox114->Text = S"00000-00000";
			// 
			// label117
			// 
			this->label117->Location = System::Drawing::Point(288, 232);
			this->label117->Name = S"label117";
			this->label117->Size = System::Drawing::Size(64, 24);
			this->label117->TabIndex = 204;
			this->label117->Text = S"REDOCTR";
			this->label117->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox115
			// 
			this->textBox115->Enabled = false;
			this->textBox115->Location = System::Drawing::Point(352, 232);
			this->textBox115->Name = S"textBox115";
			this->textBox115->Size = System::Drawing::Size(40, 20);
			this->textBox115->TabIndex = 205;
			this->textBox115->Text = S"00000";
			// 
			// groupBox18
			// 
			this->groupBox18->Controls->Add(this->textBox116);
			this->groupBox18->Controls->Add(this->label118);
			this->groupBox18->Controls->Add(this->label119);
			this->groupBox18->Controls->Add(this->textBox117);
			this->groupBox18->Controls->Add(this->label120);
			this->groupBox18->Controls->Add(this->textBox118);
			this->groupBox18->Location = System::Drawing::Point(8, 408);
			this->groupBox18->Name = S"groupBox18";
			this->groupBox18->Size = System::Drawing::Size(104, 96);
			this->groupBox18->TabIndex = 206;
			this->groupBox18->TabStop = false;
			this->groupBox18->Text = S"DAP TGT ATT";
			// 
			// textBox116
			// 
			this->textBox116->Enabled = false;
			this->textBox116->Location = System::Drawing::Point(24, 64);
			this->textBox116->Name = S"textBox116";
			this->textBox116->Size = System::Drawing::Size(72, 20);
			this->textBox116->TabIndex = 151;
			this->textBox116->Text = S"00000-00000";
			// 
			// label118
			// 
			this->label118->Location = System::Drawing::Point(8, 16);
			this->label118->Name = S"label118";
			this->label118->Size = System::Drawing::Size(16, 24);
			this->label118->TabIndex = 146;
			this->label118->Text = S"X";
			this->label118->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label119
			// 
			this->label119->Location = System::Drawing::Point(8, 40);
			this->label119->Name = S"label119";
			this->label119->Size = System::Drawing::Size(16, 24);
			this->label119->TabIndex = 147;
			this->label119->Text = S"Y";
			this->label119->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox117
			// 
			this->textBox117->Enabled = false;
			this->textBox117->Location = System::Drawing::Point(24, 40);
			this->textBox117->Name = S"textBox117";
			this->textBox117->Size = System::Drawing::Size(72, 20);
			this->textBox117->TabIndex = 150;
			this->textBox117->Text = S"00000-00000";
			// 
			// label120
			// 
			this->label120->Location = System::Drawing::Point(8, 64);
			this->label120->Name = S"label120";
			this->label120->Size = System::Drawing::Size(16, 24);
			this->label120->TabIndex = 148;
			this->label120->Text = S"Z";
			this->label120->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox118
			// 
			this->textBox118->Enabled = false;
			this->textBox118->Location = System::Drawing::Point(24, 16);
			this->textBox118->Name = S"textBox118";
			this->textBox118->Size = System::Drawing::Size(72, 20);
			this->textBox118->TabIndex = 149;
			this->textBox118->Text = S"00000-00000";
			// 
			// groupBox19
			// 
			this->groupBox19->Controls->Add(this->label129);
			this->groupBox19->Controls->Add(this->textBox127);
			this->groupBox19->Controls->Add(this->label130);
			this->groupBox19->Controls->Add(this->textBox128);
			this->groupBox19->Controls->Add(this->label128);
			this->groupBox19->Controls->Add(this->textBox126);
			this->groupBox19->Controls->Add(this->label127);
			this->groupBox19->Controls->Add(this->textBox125);
			this->groupBox19->Controls->Add(this->label126);
			this->groupBox19->Controls->Add(this->textBox124);
			this->groupBox19->Controls->Add(this->label125);
			this->groupBox19->Controls->Add(this->textBox123);
			this->groupBox19->Controls->Add(this->label124);
			this->groupBox19->Controls->Add(this->textBox122);
			this->groupBox19->Controls->Add(this->label123);
			this->groupBox19->Controls->Add(this->textBox121);
			this->groupBox19->Controls->Add(this->label122);
			this->groupBox19->Controls->Add(this->textBox120);
			this->groupBox19->Controls->Add(this->label121);
			this->groupBox19->Controls->Add(this->textBox119);
			this->groupBox19->Location = System::Drawing::Point(568, 0);
			this->groupBox19->Name = S"groupBox19";
			this->groupBox19->Size = System::Drawing::Size(88, 264);
			this->groupBox19->TabIndex = 207;
			this->groupBox19->TabStop = false;
			this->groupBox19->Text = S"I/O CHAN";
			// 
			// label129
			// 
			this->label129->Location = System::Drawing::Point(8, 184);
			this->label129->Name = S"label129";
			this->label129->Size = System::Drawing::Size(24, 24);
			this->label129->TabIndex = 198;
			this->label129->Text = S"33";
			this->label129->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox127
			// 
			this->textBox127->Enabled = false;
			this->textBox127->Location = System::Drawing::Point(40, 184);
			this->textBox127->Name = S"textBox127";
			this->textBox127->Size = System::Drawing::Size(40, 20);
			this->textBox127->TabIndex = 199;
			this->textBox127->Text = S"00000";
			// 
			// label130
			// 
			this->label130->Location = System::Drawing::Point(8, 160);
			this->label130->Name = S"label130";
			this->label130->Size = System::Drawing::Size(24, 24);
			this->label130->TabIndex = 196;
			this->label130->Text = S"32";
			this->label130->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox128
			// 
			this->textBox128->Enabled = false;
			this->textBox128->Location = System::Drawing::Point(40, 160);
			this->textBox128->Name = S"textBox128";
			this->textBox128->Size = System::Drawing::Size(40, 20);
			this->textBox128->TabIndex = 197;
			this->textBox128->Text = S"00000";
			// 
			// label128
			// 
			this->label128->Location = System::Drawing::Point(8, 136);
			this->label128->Name = S"label128";
			this->label128->Size = System::Drawing::Size(24, 24);
			this->label128->TabIndex = 194;
			this->label128->Text = S"31";
			this->label128->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox126
			// 
			this->textBox126->Enabled = false;
			this->textBox126->Location = System::Drawing::Point(40, 136);
			this->textBox126->Name = S"textBox126";
			this->textBox126->Size = System::Drawing::Size(40, 20);
			this->textBox126->TabIndex = 195;
			this->textBox126->Text = S"00000";
			// 
			// label127
			// 
			this->label127->Location = System::Drawing::Point(8, 112);
			this->label127->Name = S"label127";
			this->label127->Size = System::Drawing::Size(24, 24);
			this->label127->TabIndex = 192;
			this->label127->Text = S"30";
			this->label127->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox125
			// 
			this->textBox125->Enabled = false;
			this->textBox125->Location = System::Drawing::Point(40, 112);
			this->textBox125->Name = S"textBox125";
			this->textBox125->Size = System::Drawing::Size(40, 20);
			this->textBox125->TabIndex = 193;
			this->textBox125->Text = S"00000";
			// 
			// label126
			// 
			this->label126->Location = System::Drawing::Point(8, 88);
			this->label126->Name = S"label126";
			this->label126->Size = System::Drawing::Size(24, 24);
			this->label126->TabIndex = 190;
			this->label126->Text = S"14";
			this->label126->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox124
			// 
			this->textBox124->Enabled = false;
			this->textBox124->Location = System::Drawing::Point(40, 88);
			this->textBox124->Name = S"textBox124";
			this->textBox124->Size = System::Drawing::Size(40, 20);
			this->textBox124->TabIndex = 191;
			this->textBox124->Text = S"00000";
			// 
			// label125
			// 
			this->label125->Location = System::Drawing::Point(8, 64);
			this->label125->Name = S"label125";
			this->label125->Size = System::Drawing::Size(24, 24);
			this->label125->TabIndex = 188;
			this->label125->Text = S"13";
			this->label125->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox123
			// 
			this->textBox123->Enabled = false;
			this->textBox123->Location = System::Drawing::Point(40, 64);
			this->textBox123->Name = S"textBox123";
			this->textBox123->Size = System::Drawing::Size(40, 20);
			this->textBox123->TabIndex = 189;
			this->textBox123->Text = S"00000";
			// 
			// label124
			// 
			this->label124->Location = System::Drawing::Point(8, 40);
			this->label124->Name = S"label124";
			this->label124->Size = System::Drawing::Size(24, 24);
			this->label124->TabIndex = 186;
			this->label124->Text = S"12";
			this->label124->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox122
			// 
			this->textBox122->Enabled = false;
			this->textBox122->Location = System::Drawing::Point(40, 40);
			this->textBox122->Name = S"textBox122";
			this->textBox122->Size = System::Drawing::Size(40, 20);
			this->textBox122->TabIndex = 187;
			this->textBox122->Text = S"00000";
			// 
			// label123
			// 
			this->label123->Location = System::Drawing::Point(8, 16);
			this->label123->Name = S"label123";
			this->label123->Size = System::Drawing::Size(24, 24);
			this->label123->TabIndex = 184;
			this->label123->Text = S"11";
			this->label123->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox121
			// 
			this->textBox121->Enabled = false;
			this->textBox121->Location = System::Drawing::Point(40, 16);
			this->textBox121->Name = S"textBox121";
			this->textBox121->Size = System::Drawing::Size(40, 20);
			this->textBox121->TabIndex = 185;
			this->textBox121->Text = S"00000";
			// 
			// label122
			// 
			this->label122->Location = System::Drawing::Point(8, 232);
			this->label122->Name = S"label122";
			this->label122->Size = System::Drawing::Size(32, 24);
			this->label122->TabIndex = 182;
			this->label122->Text = S"IM33";
			this->label122->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox120
			// 
			this->textBox120->Enabled = false;
			this->textBox120->Location = System::Drawing::Point(40, 232);
			this->textBox120->Name = S"textBox120";
			this->textBox120->Size = System::Drawing::Size(40, 20);
			this->textBox120->TabIndex = 183;
			this->textBox120->Text = S"00000";
			// 
			// label121
			// 
			this->label121->Location = System::Drawing::Point(8, 208);
			this->label121->Name = S"label121";
			this->label121->Size = System::Drawing::Size(32, 24);
			this->label121->TabIndex = 180;
			this->label121->Text = S"IM30";
			this->label121->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox119
			// 
			this->textBox119->Enabled = false;
			this->textBox119->Location = System::Drawing::Point(40, 208);
			this->textBox119->Name = S"textBox119";
			this->textBox119->Size = System::Drawing::Size(40, 20);
			this->textBox119->TabIndex = 181;
			this->textBox119->Text = S"00000";
			// 
			// groupBox21
			// 
			this->groupBox21->Controls->Add(this->textBox130);
			this->groupBox21->Controls->Add(this->label132);
			this->groupBox21->Controls->Add(this->label133);
			this->groupBox21->Controls->Add(this->textBox131);
			this->groupBox21->Controls->Add(this->label134);
			this->groupBox21->Controls->Add(this->textBox132);
			this->groupBox21->Location = System::Drawing::Point(336, 384);
			this->groupBox21->Name = S"groupBox21";
			this->groupBox21->Size = System::Drawing::Size(104, 96);
			this->groupBox21->TabIndex = 209;
			this->groupBox21->TabStop = false;
			this->groupBox21->Text = S"BURN TGT PS";
			// 
			// textBox130
			// 
			this->textBox130->Enabled = false;
			this->textBox130->Location = System::Drawing::Point(24, 64);
			this->textBox130->Name = S"textBox130";
			this->textBox130->Size = System::Drawing::Size(72, 20);
			this->textBox130->TabIndex = 151;
			this->textBox130->Text = S"00000-00000";
			// 
			// label132
			// 
			this->label132->Location = System::Drawing::Point(8, 16);
			this->label132->Name = S"label132";
			this->label132->Size = System::Drawing::Size(16, 24);
			this->label132->TabIndex = 146;
			this->label132->Text = S"X";
			this->label132->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label133
			// 
			this->label133->Location = System::Drawing::Point(8, 40);
			this->label133->Name = S"label133";
			this->label133->Size = System::Drawing::Size(16, 24);
			this->label133->TabIndex = 147;
			this->label133->Text = S"Y";
			this->label133->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox131
			// 
			this->textBox131->Enabled = false;
			this->textBox131->Location = System::Drawing::Point(24, 40);
			this->textBox131->Name = S"textBox131";
			this->textBox131->Size = System::Drawing::Size(72, 20);
			this->textBox131->TabIndex = 150;
			this->textBox131->Text = S"00000-00000";
			// 
			// label134
			// 
			this->label134->Location = System::Drawing::Point(8, 64);
			this->label134->Name = S"label134";
			this->label134->Size = System::Drawing::Size(16, 24);
			this->label134->TabIndex = 148;
			this->label134->Text = S"Z";
			this->label134->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox132
			// 
			this->textBox132->Enabled = false;
			this->textBox132->Location = System::Drawing::Point(24, 16);
			this->textBox132->Name = S"textBox132";
			this->textBox132->Size = System::Drawing::Size(72, 20);
			this->textBox132->TabIndex = 149;
			this->textBox132->Text = S"00000-00000";
			// 
			// groupBox20
			// 
			this->groupBox20->Controls->Add(this->label75);
			this->groupBox20->Controls->Add(this->label70);
			this->groupBox20->Controls->Add(this->label69);
			this->groupBox20->Controls->Add(this->textBox137);
			this->groupBox20->Controls->Add(this->label138);
			this->groupBox20->Controls->Add(this->textBox136);
			this->groupBox20->Controls->Add(this->label137);
			this->groupBox20->Controls->Add(this->textBox135);
			this->groupBox20->Controls->Add(this->label136);
			this->groupBox20->Controls->Add(this->textBox134);
			this->groupBox20->Controls->Add(this->label135);
			this->groupBox20->Controls->Add(this->textBox133);
			this->groupBox20->Controls->Add(this->label131);
			this->groupBox20->Controls->Add(this->textBox129);
			this->groupBox20->Controls->Add(this->label2);
			this->groupBox20->Controls->Add(this->label72);
			this->groupBox20->Location = System::Drawing::Point(344, 32);
			this->groupBox20->Name = S"groupBox20";
			this->groupBox20->Size = System::Drawing::Size(128, 192);
			this->groupBox20->TabIndex = 210;
			this->groupBox20->TabStop = false;
			this->groupBox20->Text = S"DSKY";
			// 
			// label75
			// 
			this->label75->Location = System::Drawing::Point(56, 160);
			this->label75->Name = S"label75";
			this->label75->Size = System::Drawing::Size(64, 24);
			this->label75->TabIndex = 179;
			this->label75->Text = S"TRACKER";
			this->label75->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label70
			// 
			this->label70->Location = System::Drawing::Point(8, 160);
			this->label70->Name = S"label70";
			this->label70->Size = System::Drawing::Size(48, 24);
			this->label70->TabIndex = 173;
			this->label70->Text = S"GIMBAL LOCK";
			this->label70->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label69
			// 
			this->label69->Location = System::Drawing::Point(8, 136);
			this->label69->Name = S"label69";
			this->label69->Size = System::Drawing::Size(48, 24);
			this->label69->TabIndex = 172;
			this->label69->Text = S"NO ATT";
			this->label69->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox137
			// 
			this->textBox137->Enabled = false;
			this->textBox137->Location = System::Drawing::Point(72, 112);
			this->textBox137->Name = S"textBox137";
			this->textBox137->Size = System::Drawing::Size(48, 20);
			this->textBox137->TabIndex = 158;
			this->textBox137->Text = S"+00000";
			// 
			// label138
			// 
			this->label138->Location = System::Drawing::Point(8, 112);
			this->label138->Name = S"label138";
			this->label138->Size = System::Drawing::Size(40, 24);
			this->label138->TabIndex = 157;
			this->label138->Text = S"R3";
			this->label138->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox136
			// 
			this->textBox136->Enabled = false;
			this->textBox136->Location = System::Drawing::Point(72, 88);
			this->textBox136->Name = S"textBox136";
			this->textBox136->Size = System::Drawing::Size(48, 20);
			this->textBox136->TabIndex = 156;
			this->textBox136->Text = S"+00000";
			// 
			// label137
			// 
			this->label137->Location = System::Drawing::Point(8, 88);
			this->label137->Name = S"label137";
			this->label137->Size = System::Drawing::Size(40, 24);
			this->label137->TabIndex = 155;
			this->label137->Text = S"R2";
			this->label137->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox135
			// 
			this->textBox135->Enabled = false;
			this->textBox135->Location = System::Drawing::Point(72, 64);
			this->textBox135->Name = S"textBox135";
			this->textBox135->Size = System::Drawing::Size(48, 20);
			this->textBox135->TabIndex = 154;
			this->textBox135->Text = S"+00000";
			// 
			// label136
			// 
			this->label136->Location = System::Drawing::Point(8, 64);
			this->label136->Name = S"label136";
			this->label136->Size = System::Drawing::Size(40, 24);
			this->label136->TabIndex = 153;
			this->label136->Text = S"R1";
			this->label136->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox134
			// 
			this->textBox134->Enabled = false;
			this->textBox134->Location = System::Drawing::Point(96, 40);
			this->textBox134->Name = S"textBox134";
			this->textBox134->Size = System::Drawing::Size(24, 20);
			this->textBox134->TabIndex = 152;
			this->textBox134->Text = S"00";
			// 
			// label135
			// 
			this->label135->Location = System::Drawing::Point(72, 40);
			this->label135->Name = S"label135";
			this->label135->Size = System::Drawing::Size(16, 24);
			this->label135->TabIndex = 151;
			this->label135->Text = S"N";
			this->label135->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox133
			// 
			this->textBox133->Enabled = false;
			this->textBox133->Location = System::Drawing::Point(32, 40);
			this->textBox133->Name = S"textBox133";
			this->textBox133->Size = System::Drawing::Size(24, 20);
			this->textBox133->TabIndex = 150;
			this->textBox133->Text = S"00";
			// 
			// label131
			// 
			this->label131->Location = System::Drawing::Point(8, 40);
			this->label131->Name = S"label131";
			this->label131->Size = System::Drawing::Size(16, 24);
			this->label131->TabIndex = 149;
			this->label131->Text = S"V";
			this->label131->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox129
			// 
			this->textBox129->Enabled = false;
			this->textBox129->Location = System::Drawing::Point(96, 16);
			this->textBox129->Name = S"textBox129";
			this->textBox129->Size = System::Drawing::Size(24, 20);
			this->textBox129->TabIndex = 148;
			this->textBox129->Text = S"00";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 16);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(40, 24);
			this->label2->TabIndex = 147;
			this->label2->Text = S"PROG";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label72
			// 
			this->label72->ForeColor = System::Drawing::SystemColors::ControlText;
			this->label72->Location = System::Drawing::Point(56, 136);
			this->label72->Name = S"label72";
			this->label72->Size = System::Drawing::Size(56, 24);
			this->label72->TabIndex = 175;
			this->label72->Text = S"PROG";
			this->label72->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox11
			// 
			this->groupBox11->Controls->Add(this->textBox158);
			this->groupBox11->Controls->Add(this->label163);
			this->groupBox11->Controls->Add(this->textBox159);
			this->groupBox11->Controls->Add(this->label164);
			this->groupBox11->Controls->Add(this->label157);
			this->groupBox11->Controls->Add(this->textBox152);
			this->groupBox11->Controls->Add(this->textBox141);
			this->groupBox11->Controls->Add(this->label146);
			this->groupBox11->Controls->Add(this->textBox142);
			this->groupBox11->Controls->Add(this->label147);
			this->groupBox11->Controls->Add(this->textBox143);
			this->groupBox11->Controls->Add(this->label148);
			this->groupBox11->Controls->Add(this->textBox139);
			this->groupBox11->Controls->Add(this->label144);
			this->groupBox11->Controls->Add(this->textBox140);
			this->groupBox11->Controls->Add(this->label145);
			this->groupBox11->Controls->Add(this->textBox74);
			this->groupBox11->Controls->Add(this->label140);
			this->groupBox11->Controls->Add(this->textBox73);
			this->groupBox11->Controls->Add(this->label139);
			this->groupBox11->Controls->Add(this->textBox68);
			this->groupBox11->Controls->Add(this->label74);
			this->groupBox11->Controls->Add(this->textBox69);
			this->groupBox11->Controls->Add(this->label76);
			this->groupBox11->Controls->Add(this->textBox70);
			this->groupBox11->Controls->Add(this->label77);
			this->groupBox11->Controls->Add(this->textBox67);
			this->groupBox11->Controls->Add(this->label73);
			this->groupBox11->Controls->Add(this->textBox66);
			this->groupBox11->Controls->Add(this->label71);
			this->groupBox11->Controls->Add(this->textBox65);
			this->groupBox11->Controls->Add(this->label68);
			this->groupBox11->Controls->Add(this->label26);
			this->groupBox11->Controls->Add(this->textBox24);
			this->groupBox11->Location = System::Drawing::Point(608, 264);
			this->groupBox11->Name = S"groupBox11";
			this->groupBox11->Size = System::Drawing::Size(144, 432);
			this->groupBox11->TabIndex = 211;
			this->groupBox11->TabStop = false;
			this->groupBox11->Text = S"BURN DATA";
			// 
			// textBox158
			// 
			this->textBox158->Enabled = false;
			this->textBox158->Location = System::Drawing::Point(64, 400);
			this->textBox158->Name = S"textBox158";
			this->textBox158->Size = System::Drawing::Size(72, 20);
			this->textBox158->TabIndex = 188;
			this->textBox158->Text = S"00000-00000";
			// 
			// label163
			// 
			this->label163->Location = System::Drawing::Point(8, 400);
			this->label163->Name = S"label163";
			this->label163->Size = System::Drawing::Size(56, 24);
			this->label163->TabIndex = 187;
			this->label163->Text = S"ALP";
			this->label163->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox159
			// 
			this->textBox159->Enabled = false;
			this->textBox159->Location = System::Drawing::Point(64, 376);
			this->textBox159->Name = S"textBox159";
			this->textBox159->Size = System::Drawing::Size(72, 20);
			this->textBox159->TabIndex = 186;
			this->textBox159->Text = S"00000-00000";
			// 
			// label164
			// 
			this->label164->Location = System::Drawing::Point(8, 376);
			this->label164->Name = S"label164";
			this->label164->Size = System::Drawing::Size(56, 24);
			this->label164->TabIndex = 185;
			this->label164->Text = S"CSTEER";
			this->label164->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label157
			// 
			this->label157->Location = System::Drawing::Point(8, 40);
			this->label157->Name = S"label157";
			this->label157->Size = System::Drawing::Size(32, 24);
			this->label157->TabIndex = 183;
			this->label157->Text = S"TGO";
			this->label157->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox152
			// 
			this->textBox152->Enabled = false;
			this->textBox152->Location = System::Drawing::Point(64, 40);
			this->textBox152->Name = S"textBox152";
			this->textBox152->Size = System::Drawing::Size(72, 20);
			this->textBox152->TabIndex = 184;
			this->textBox152->Text = S"00000-00000";
			// 
			// textBox141
			// 
			this->textBox141->Enabled = false;
			this->textBox141->Location = System::Drawing::Point(64, 352);
			this->textBox141->Name = S"textBox141";
			this->textBox141->Size = System::Drawing::Size(72, 20);
			this->textBox141->TabIndex = 182;
			this->textBox141->Text = S"00000-00000";
			// 
			// label146
			// 
			this->label146->Location = System::Drawing::Point(8, 352);
			this->label146->Name = S"label146";
			this->label146->Size = System::Drawing::Size(56, 24);
			this->label146->TabIndex = 181;
			this->label146->Text = S"DELTAR";
			this->label146->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox142
			// 
			this->textBox142->Enabled = false;
			this->textBox142->Location = System::Drawing::Point(64, 328);
			this->textBox142->Name = S"textBox142";
			this->textBox142->Size = System::Drawing::Size(72, 20);
			this->textBox142->TabIndex = 180;
			this->textBox142->Text = S"00000-00000";
			// 
			// label147
			// 
			this->label147->Location = System::Drawing::Point(8, 328);
			this->label147->Name = S"label147";
			this->label147->Size = System::Drawing::Size(56, 24);
			this->label147->TabIndex = 179;
			this->label147->Text = S"CNTANG";
			this->label147->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox143
			// 
			this->textBox143->Enabled = false;
			this->textBox143->Location = System::Drawing::Point(64, 304);
			this->textBox143->Name = S"textBox143";
			this->textBox143->Size = System::Drawing::Size(72, 20);
			this->textBox143->TabIndex = 178;
			this->textBox143->Text = S"00000-00000";
			// 
			// label148
			// 
			this->label148->Location = System::Drawing::Point(8, 304);
			this->label148->Name = S"label148";
			this->label148->Size = System::Drawing::Size(56, 24);
			this->label148->TabIndex = 177;
			this->label148->Text = S"ELEV";
			this->label148->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox139
			// 
			this->textBox139->Enabled = false;
			this->textBox139->Location = System::Drawing::Point(64, 280);
			this->textBox139->Name = S"textBox139";
			this->textBox139->Size = System::Drawing::Size(72, 20);
			this->textBox139->TabIndex = 176;
			this->textBox139->Text = S"00000-00000";
			// 
			// label144
			// 
			this->label144->Location = System::Drawing::Point(8, 280);
			this->label144->Name = S"label144";
			this->label144->Size = System::Drawing::Size(56, 24);
			this->label144->TabIndex = 175;
			this->label144->Text = S"RRATE";
			this->label144->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox140
			// 
			this->textBox140->Enabled = false;
			this->textBox140->Location = System::Drawing::Point(64, 256);
			this->textBox140->Name = S"textBox140";
			this->textBox140->Size = System::Drawing::Size(72, 20);
			this->textBox140->TabIndex = 174;
			this->textBox140->Text = S"00000-00000";
			// 
			// label145
			// 
			this->label145->Location = System::Drawing::Point(8, 256);
			this->label145->Name = S"label145";
			this->label145->Size = System::Drawing::Size(56, 24);
			this->label145->TabIndex = 173;
			this->label145->Text = S"RANGE";
			this->label145->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox74
			// 
			this->textBox74->Enabled = false;
			this->textBox74->Location = System::Drawing::Point(64, 232);
			this->textBox74->Name = S"textBox74";
			this->textBox74->Size = System::Drawing::Size(72, 20);
			this->textBox74->TabIndex = 172;
			this->textBox74->Text = S"00000-00000";
			// 
			// label140
			// 
			this->label140->Location = System::Drawing::Point(8, 232);
			this->label140->Name = S"label140";
			this->label140->Size = System::Drawing::Size(56, 24);
			this->label140->TabIndex = 171;
			this->label140->Text = S"TPASS4";
			this->label140->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox73
			// 
			this->textBox73->Enabled = false;
			this->textBox73->Location = System::Drawing::Point(64, 208);
			this->textBox73->Name = S"textBox73";
			this->textBox73->Size = System::Drawing::Size(72, 20);
			this->textBox73->TabIndex = 170;
			this->textBox73->Text = S"00000-00000";
			// 
			// label139
			// 
			this->label139->Location = System::Drawing::Point(8, 208);
			this->label139->Name = S"label139";
			this->label139->Size = System::Drawing::Size(56, 24);
			this->label139->TabIndex = 169;
			this->label139->Text = S"DELVTPF";
			this->label139->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox68
			// 
			this->textBox68->Enabled = false;
			this->textBox68->Location = System::Drawing::Point(64, 184);
			this->textBox68->Name = S"textBox68";
			this->textBox68->Size = System::Drawing::Size(72, 20);
			this->textBox68->TabIndex = 168;
			this->textBox68->Text = S"00000-00000";
			// 
			// label74
			// 
			this->label74->Location = System::Drawing::Point(8, 184);
			this->label74->Name = S"label74";
			this->label74->Size = System::Drawing::Size(56, 24);
			this->label74->TabIndex = 167;
			this->label74->Text = S"TTPI";
			this->label74->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox69
			// 
			this->textBox69->Enabled = false;
			this->textBox69->Location = System::Drawing::Point(64, 160);
			this->textBox69->Name = S"textBox69";
			this->textBox69->Size = System::Drawing::Size(72, 20);
			this->textBox69->TabIndex = 166;
			this->textBox69->Text = S"00000-00000";
			// 
			// label76
			// 
			this->label76->Location = System::Drawing::Point(8, 160);
			this->label76->Name = S"label76";
			this->label76->Size = System::Drawing::Size(56, 24);
			this->label76->TabIndex = 165;
			this->label76->Text = S"MRKCNT";
			this->label76->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox70
			// 
			this->textBox70->Enabled = false;
			this->textBox70->Location = System::Drawing::Point(64, 136);
			this->textBox70->Name = S"textBox70";
			this->textBox70->Size = System::Drawing::Size(72, 20);
			this->textBox70->TabIndex = 164;
			this->textBox70->Text = S"00000-00000";
			// 
			// label77
			// 
			this->label77->Location = System::Drawing::Point(8, 136);
			this->label77->Name = S"label77";
			this->label77->Size = System::Drawing::Size(56, 24);
			this->label77->TabIndex = 163;
			this->label77->Text = S"VHFCNT";
			this->label77->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox67
			// 
			this->textBox67->Enabled = false;
			this->textBox67->Location = System::Drawing::Point(64, 112);
			this->textBox67->Name = S"textBox67";
			this->textBox67->Size = System::Drawing::Size(72, 20);
			this->textBox67->TabIndex = 162;
			this->textBox67->Text = S"00000-00000";
			// 
			// label73
			// 
			this->label73->Location = System::Drawing::Point(8, 112);
			this->label73->Name = S"label73";
			this->label73->Size = System::Drawing::Size(56, 24);
			this->label73->TabIndex = 161;
			this->label73->Text = S"RM";
			this->label73->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox66
			// 
			this->textBox66->Enabled = false;
			this->textBox66->Location = System::Drawing::Point(64, 88);
			this->textBox66->Name = S"textBox66";
			this->textBox66->Size = System::Drawing::Size(72, 20);
			this->textBox66->TabIndex = 160;
			this->textBox66->Text = S"00000-00000";
			// 
			// label71
			// 
			this->label71->Location = System::Drawing::Point(8, 88);
			this->label71->Name = S"label71";
			this->label71->Size = System::Drawing::Size(56, 24);
			this->label71->TabIndex = 159;
			this->label71->Text = S"VHFTIME";
			this->label71->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox65
			// 
			this->textBox65->Enabled = false;
			this->textBox65->Location = System::Drawing::Point(64, 64);
			this->textBox65->Name = S"textBox65";
			this->textBox65->Size = System::Drawing::Size(72, 20);
			this->textBox65->TabIndex = 158;
			this->textBox65->Text = S"00000-00000";
			// 
			// label68
			// 
			this->label68->Location = System::Drawing::Point(8, 64);
			this->label68->Name = S"label68";
			this->label68->Size = System::Drawing::Size(48, 24);
			this->label68->TabIndex = 157;
			this->label68->Text = S"DELLT4";
			this->label68->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label67
			// 
			this->label67->Location = System::Drawing::Point(656, 128);
			this->label67->Name = S"label67";
			this->label67->Size = System::Drawing::Size(72, 24);
			this->label67->TabIndex = 212;
			this->label67->Text = S"ECSTEER";
			this->label67->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox71
			// 
			this->textBox71->Enabled = false;
			this->textBox71->Location = System::Drawing::Point(728, 128);
			this->textBox71->Name = S"textBox71";
			this->textBox71->Size = System::Drawing::Size(40, 20);
			this->textBox71->TabIndex = 213;
			this->textBox71->Text = S"00000";
			// 
			// label78
			// 
			this->label78->Location = System::Drawing::Point(656, 200);
			this->label78->Name = S"label78";
			this->label78->Size = System::Drawing::Size(40, 24);
			this->label78->TabIndex = 214;
			this->label78->Text = S"LSAT";
			this->label78->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox72
			// 
			this->textBox72->Enabled = false;
			this->textBox72->Location = System::Drawing::Point(728, 200);
			this->textBox72->Name = S"textBox72";
			this->textBox72->Size = System::Drawing::Size(40, 20);
			this->textBox72->TabIndex = 215;
			this->textBox72->Text = S"00000";
			// 
			// groupBox22
			// 
			this->groupBox22->Controls->Add(this->textBox75);
			this->groupBox22->Controls->Add(this->label141);
			this->groupBox22->Controls->Add(this->label142);
			this->groupBox22->Controls->Add(this->textBox76);
			this->groupBox22->Controls->Add(this->label143);
			this->groupBox22->Controls->Add(this->textBox138);
			this->groupBox22->Location = System::Drawing::Point(504, 408);
			this->groupBox22->Name = S"groupBox22";
			this->groupBox22->Size = System::Drawing::Size(104, 96);
			this->groupBox22->TabIndex = 216;
			this->groupBox22->TabStop = false;
			this->groupBox22->Text = S"BURN DV";
			// 
			// textBox75
			// 
			this->textBox75->Enabled = false;
			this->textBox75->Location = System::Drawing::Point(24, 64);
			this->textBox75->Name = S"textBox75";
			this->textBox75->Size = System::Drawing::Size(72, 20);
			this->textBox75->TabIndex = 151;
			this->textBox75->Text = S"00000-00000";
			// 
			// label141
			// 
			this->label141->Location = System::Drawing::Point(8, 16);
			this->label141->Name = S"label141";
			this->label141->Size = System::Drawing::Size(16, 24);
			this->label141->TabIndex = 146;
			this->label141->Text = S"X";
			this->label141->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label142
			// 
			this->label142->Location = System::Drawing::Point(8, 40);
			this->label142->Name = S"label142";
			this->label142->Size = System::Drawing::Size(16, 24);
			this->label142->TabIndex = 147;
			this->label142->Text = S"Y";
			this->label142->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox76
			// 
			this->textBox76->Enabled = false;
			this->textBox76->Location = System::Drawing::Point(24, 40);
			this->textBox76->Name = S"textBox76";
			this->textBox76->Size = System::Drawing::Size(72, 20);
			this->textBox76->TabIndex = 150;
			this->textBox76->Text = S"00000-00000";
			// 
			// label143
			// 
			this->label143->Location = System::Drawing::Point(8, 64);
			this->label143->Name = S"label143";
			this->label143->Size = System::Drawing::Size(16, 24);
			this->label143->TabIndex = 148;
			this->label143->Text = S"Z";
			this->label143->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox138
			// 
			this->textBox138->Enabled = false;
			this->textBox138->Location = System::Drawing::Point(24, 16);
			this->textBox138->Name = S"textBox138";
			this->textBox138->Size = System::Drawing::Size(72, 20);
			this->textBox138->TabIndex = 149;
			this->textBox138->Text = S"00000-00000";
			// 
			// groupBox23
			// 
			this->groupBox23->Controls->Add(this->textBox144);
			this->groupBox23->Controls->Add(this->label149);
			this->groupBox23->Controls->Add(this->label150);
			this->groupBox23->Controls->Add(this->textBox145);
			this->groupBox23->Controls->Add(this->label151);
			this->groupBox23->Controls->Add(this->textBox146);
			this->groupBox23->Location = System::Drawing::Point(504, 504);
			this->groupBox23->Name = S"groupBox23";
			this->groupBox23->Size = System::Drawing::Size(104, 96);
			this->groupBox23->TabIndex = 217;
			this->groupBox23->TabStop = false;
			this->groupBox23->Text = S"BURN DV REM";
			// 
			// textBox144
			// 
			this->textBox144->Enabled = false;
			this->textBox144->Location = System::Drawing::Point(24, 64);
			this->textBox144->Name = S"textBox144";
			this->textBox144->Size = System::Drawing::Size(72, 20);
			this->textBox144->TabIndex = 151;
			this->textBox144->Text = S"00000-00000";
			// 
			// label149
			// 
			this->label149->Location = System::Drawing::Point(8, 16);
			this->label149->Name = S"label149";
			this->label149->Size = System::Drawing::Size(16, 24);
			this->label149->TabIndex = 146;
			this->label149->Text = S"X";
			this->label149->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label150
			// 
			this->label150->Location = System::Drawing::Point(8, 40);
			this->label150->Name = S"label150";
			this->label150->Size = System::Drawing::Size(16, 24);
			this->label150->TabIndex = 147;
			this->label150->Text = S"Y";
			this->label150->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox145
			// 
			this->textBox145->Enabled = false;
			this->textBox145->Location = System::Drawing::Point(24, 40);
			this->textBox145->Name = S"textBox145";
			this->textBox145->Size = System::Drawing::Size(72, 20);
			this->textBox145->TabIndex = 150;
			this->textBox145->Text = S"00000-00000";
			// 
			// label151
			// 
			this->label151->Location = System::Drawing::Point(8, 64);
			this->label151->Name = S"label151";
			this->label151->Size = System::Drawing::Size(16, 24);
			this->label151->TabIndex = 148;
			this->label151->Text = S"Z";
			this->label151->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox146
			// 
			this->textBox146->Enabled = false;
			this->textBox146->Location = System::Drawing::Point(24, 16);
			this->textBox146->Name = S"textBox146";
			this->textBox146->Size = System::Drawing::Size(72, 20);
			this->textBox146->TabIndex = 149;
			this->textBox146->Text = S"00000-00000";
			// 
			// groupBox24
			// 
			this->groupBox24->Controls->Add(this->textBox179);
			this->groupBox24->Controls->Add(this->label184);
			this->groupBox24->Controls->Add(this->textBox178);
			this->groupBox24->Controls->Add(this->label183);
			this->groupBox24->Controls->Add(this->textBox177);
			this->groupBox24->Controls->Add(this->label182);
			this->groupBox24->Controls->Add(this->textBox176);
			this->groupBox24->Controls->Add(this->label181);
			this->groupBox24->Controls->Add(this->textBox175);
			this->groupBox24->Controls->Add(this->label180);
			this->groupBox24->Controls->Add(this->textBox170);
			this->groupBox24->Controls->Add(this->label175);
			this->groupBox24->Controls->Add(this->textBox171);
			this->groupBox24->Controls->Add(this->label176);
			this->groupBox24->Controls->Add(this->textBox167);
			this->groupBox24->Controls->Add(this->label172);
			this->groupBox24->Controls->Add(this->textBox168);
			this->groupBox24->Controls->Add(this->label173);
			this->groupBox24->Controls->Add(this->textBox169);
			this->groupBox24->Controls->Add(this->label174);
			this->groupBox24->Controls->Add(this->textBox165);
			this->groupBox24->Controls->Add(this->label170);
			this->groupBox24->Controls->Add(this->textBox166);
			this->groupBox24->Controls->Add(this->label171);
			this->groupBox24->Controls->Add(this->textBox164);
			this->groupBox24->Controls->Add(this->label169);
			this->groupBox24->Controls->Add(this->textBox160);
			this->groupBox24->Controls->Add(this->label165);
			this->groupBox24->Controls->Add(this->textBox151);
			this->groupBox24->Controls->Add(this->label156);
			this->groupBox24->Controls->Add(this->textBox150);
			this->groupBox24->Controls->Add(this->label155);
			this->groupBox24->Controls->Add(this->textBox149);
			this->groupBox24->Controls->Add(this->label154);
			this->groupBox24->Controls->Add(this->textBox148);
			this->groupBox24->Controls->Add(this->label153);
			this->groupBox24->Controls->Add(this->textBox147);
			this->groupBox24->Controls->Add(this->label152);
			this->groupBox24->Location = System::Drawing::Point(848, 216);
			this->groupBox24->Name = S"groupBox24";
			this->groupBox24->Size = System::Drawing::Size(160, 480);
			this->groupBox24->TabIndex = 218;
			this->groupBox24->TabStop = false;
			this->groupBox24->Text = S"RE-ENTRY";
			// 
			// textBox179
			// 
			this->textBox179->Enabled = false;
			this->textBox179->Location = System::Drawing::Point(80, 88);
			this->textBox179->Name = S"textBox179";
			this->textBox179->Size = System::Drawing::Size(72, 20);
			this->textBox179->TabIndex = 218;
			this->textBox179->Text = S"00000-00000";
			// 
			// label184
			// 
			this->label184->Location = System::Drawing::Point(8, 88);
			this->label184->Name = S"label184";
			this->label184->Size = System::Drawing::Size(56, 24);
			this->label184->TabIndex = 217;
			this->label184->Text = S"RTGO";
			this->label184->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox178
			// 
			this->textBox178->Enabled = false;
			this->textBox178->Location = System::Drawing::Point(80, 256);
			this->textBox178->Name = S"textBox178";
			this->textBox178->Size = System::Drawing::Size(72, 20);
			this->textBox178->TabIndex = 216;
			this->textBox178->Text = S"00000-00000";
			// 
			// label183
			// 
			this->label183->Location = System::Drawing::Point(8, 256);
			this->label183->Name = S"label183";
			this->label183->Size = System::Drawing::Size(72, 24);
			this->label183->TabIndex = 215;
			this->label183->Text = S"ROLLC";
			this->label183->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox177
			// 
			this->textBox177->Enabled = false;
			this->textBox177->Location = System::Drawing::Point(80, 448);
			this->textBox177->Name = S"textBox177";
			this->textBox177->Size = System::Drawing::Size(72, 20);
			this->textBox177->TabIndex = 214;
			this->textBox177->Text = S"00000-00000";
			// 
			// label182
			// 
			this->label182->Location = System::Drawing::Point(8, 448);
			this->label182->Name = S"label182";
			this->label182->Size = System::Drawing::Size(72, 24);
			this->label182->TabIndex = 213;
			this->label182->Text = S"VPRED";
			this->label182->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox176
			// 
			this->textBox176->Enabled = false;
			this->textBox176->Location = System::Drawing::Point(80, 424);
			this->textBox176->Name = S"textBox176";
			this->textBox176->Size = System::Drawing::Size(72, 20);
			this->textBox176->TabIndex = 212;
			this->textBox176->Text = S"00000-00000";
			// 
			// label181
			// 
			this->label181->Location = System::Drawing::Point(8, 424);
			this->label181->Name = S"label181";
			this->label181->Size = System::Drawing::Size(72, 24);
			this->label181->TabIndex = 211;
			this->label181->Text = S"VIO";
			this->label181->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox175
			// 
			this->textBox175->Enabled = false;
			this->textBox175->Location = System::Drawing::Point(80, 400);
			this->textBox175->Name = S"textBox175";
			this->textBox175->Size = System::Drawing::Size(72, 20);
			this->textBox175->TabIndex = 210;
			this->textBox175->Text = S"00000-00000";
			// 
			// label180
			// 
			this->label180->Location = System::Drawing::Point(8, 400);
			this->label180->Name = S"label180";
			this->label180->Size = System::Drawing::Size(72, 24);
			this->label180->TabIndex = 209;
			this->label180->Text = S"TTE";
			this->label180->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox170
			// 
			this->textBox170->Enabled = false;
			this->textBox170->Location = System::Drawing::Point(80, 376);
			this->textBox170->Name = S"textBox170";
			this->textBox170->Size = System::Drawing::Size(72, 20);
			this->textBox170->TabIndex = 208;
			this->textBox170->Text = S"00000-00000";
			// 
			// label175
			// 
			this->label175->Location = System::Drawing::Point(8, 376);
			this->label175->Name = S"label175";
			this->label175->Size = System::Drawing::Size(72, 24);
			this->label175->TabIndex = 207;
			this->label175->Text = S"BETA/180";
			this->label175->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox171
			// 
			this->textBox171->Enabled = false;
			this->textBox171->Location = System::Drawing::Point(80, 352);
			this->textBox171->Name = S"textBox171";
			this->textBox171->Size = System::Drawing::Size(72, 20);
			this->textBox171->TabIndex = 206;
			this->textBox171->Text = S"00000-00000";
			// 
			// label176
			// 
			this->label176->Location = System::Drawing::Point(8, 352);
			this->label176->Name = S"label176";
			this->label176->Size = System::Drawing::Size(72, 24);
			this->label176->TabIndex = 205;
			this->label176->Text = S"ALFA/180";
			this->label176->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox167
			// 
			this->textBox167->Enabled = false;
			this->textBox167->Location = System::Drawing::Point(80, 328);
			this->textBox167->Name = S"textBox167";
			this->textBox167->Size = System::Drawing::Size(72, 20);
			this->textBox167->TabIndex = 204;
			this->textBox167->Text = S"00000-00000";
			// 
			// label172
			// 
			this->label172->Location = System::Drawing::Point(8, 328);
			this->label172->Name = S"label172";
			this->label172->Size = System::Drawing::Size(72, 24);
			this->label172->TabIndex = 203;
			this->label172->Text = S"THETA H";
			this->label172->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox168
			// 
			this->textBox168->Enabled = false;
			this->textBox168->Location = System::Drawing::Point(80, 304);
			this->textBox168->Name = S"textBox168";
			this->textBox168->Size = System::Drawing::Size(72, 20);
			this->textBox168->TabIndex = 202;
			this->textBox168->Text = S"00000-00000";
			// 
			// label173
			// 
			this->label173->Location = System::Drawing::Point(8, 304);
			this->label173->Name = S"label173";
			this->label173->Size = System::Drawing::Size(72, 24);
			this->label173->TabIndex = 201;
			this->label173->Text = S"RDOT";
			this->label173->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox169
			// 
			this->textBox169->Enabled = false;
			this->textBox169->Location = System::Drawing::Point(80, 280);
			this->textBox169->Name = S"textBox169";
			this->textBox169->Size = System::Drawing::Size(72, 20);
			this->textBox169->TabIndex = 200;
			this->textBox169->Text = S"00000-00000";
			// 
			// label174
			// 
			this->label174->Location = System::Drawing::Point(8, 280);
			this->label174->Name = S"label174";
			this->label174->Size = System::Drawing::Size(72, 24);
			this->label174->TabIndex = 199;
			this->label174->Text = S"LATANG";
			this->label174->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox165
			// 
			this->textBox165->Enabled = false;
			this->textBox165->Location = System::Drawing::Point(80, 232);
			this->textBox165->Name = S"textBox165";
			this->textBox165->Size = System::Drawing::Size(72, 20);
			this->textBox165->TabIndex = 198;
			this->textBox165->Text = S"00000-00000";
			// 
			// label170
			// 
			this->label170->Location = System::Drawing::Point(8, 232);
			this->label170->Name = S"label170";
			this->label170->Size = System::Drawing::Size(72, 24);
			this->label170->TabIndex = 197;
			this->label170->Text = S"ROLLTM";
			this->label170->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox166
			// 
			this->textBox166->Enabled = false;
			this->textBox166->Location = System::Drawing::Point(80, 208);
			this->textBox166->Name = S"textBox166";
			this->textBox166->Size = System::Drawing::Size(72, 20);
			this->textBox166->TabIndex = 196;
			this->textBox166->Text = S"00000-00000";
			// 
			// label171
			// 
			this->label171->Location = System::Drawing::Point(8, 208);
			this->label171->Name = S"label171";
			this->label171->Size = System::Drawing::Size(72, 24);
			this->label171->TabIndex = 195;
			this->label171->Text = S"PAXERRI";
			this->label171->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox164
			// 
			this->textBox164->Enabled = false;
			this->textBox164->Location = System::Drawing::Point(80, 184);
			this->textBox164->Name = S"textBox164";
			this->textBox164->Size = System::Drawing::Size(72, 20);
			this->textBox164->TabIndex = 194;
			this->textBox164->Text = S"00000-00000";
			// 
			// label169
			// 
			this->label169->Location = System::Drawing::Point(8, 184);
			this->label169->Name = S"label169";
			this->label169->Size = System::Drawing::Size(72, 24);
			this->label169->TabIndex = 193;
			this->label169->Text = S"DSRD L/D";
			this->label169->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox160
			// 
			this->textBox160->Enabled = false;
			this->textBox160->Location = System::Drawing::Point(80, 160);
			this->textBox160->Name = S"textBox160";
			this->textBox160->Size = System::Drawing::Size(72, 20);
			this->textBox160->TabIndex = 192;
			this->textBox160->Text = S"00000-00000";
			// 
			// label165
			// 
			this->label165->Location = System::Drawing::Point(8, 160);
			this->label165->Name = S"label165";
			this->label165->Size = System::Drawing::Size(72, 24);
			this->label165->TabIndex = 191;
			this->label165->Text = S"CMDAPMOD";
			this->label165->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox151
			// 
			this->textBox151->Enabled = false;
			this->textBox151->Location = System::Drawing::Point(80, 136);
			this->textBox151->Name = S"textBox151";
			this->textBox151->Size = System::Drawing::Size(72, 20);
			this->textBox151->TabIndex = 190;
			this->textBox151->Text = S"00000-00000";
			// 
			// label156
			// 
			this->label156->Location = System::Drawing::Point(8, 136);
			this->label156->Name = S"label156";
			this->label156->Size = System::Drawing::Size(72, 24);
			this->label156->TabIndex = 189;
			this->label156->Text = S"GAMMA EI";
			this->label156->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox150
			// 
			this->textBox150->Enabled = false;
			this->textBox150->Location = System::Drawing::Point(80, 112);
			this->textBox150->Name = S"textBox150";
			this->textBox150->Size = System::Drawing::Size(72, 20);
			this->textBox150->TabIndex = 188;
			this->textBox150->Text = S"00000-00000";
			// 
			// label155
			// 
			this->label155->Location = System::Drawing::Point(8, 112);
			this->label155->Name = S"label155";
			this->label155->Size = System::Drawing::Size(56, 24);
			this->label155->TabIndex = 187;
			this->label155->Text = S"VPRED";
			this->label155->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox149
			// 
			this->textBox149->Enabled = false;
			this->textBox149->Location = System::Drawing::Point(80, 64);
			this->textBox149->Name = S"textBox149";
			this->textBox149->Size = System::Drawing::Size(72, 20);
			this->textBox149->TabIndex = 186;
			this->textBox149->Text = S"00000-00000";
			// 
			// label154
			// 
			this->label154->Location = System::Drawing::Point(8, 64);
			this->label154->Name = S"label154";
			this->label154->Size = System::Drawing::Size(72, 24);
			this->label154->TabIndex = 185;
			this->label154->Text = S"LONGITUDE";
			this->label154->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox148
			// 
			this->textBox148->Enabled = false;
			this->textBox148->Location = System::Drawing::Point(80, 40);
			this->textBox148->Name = S"textBox148";
			this->textBox148->Size = System::Drawing::Size(72, 20);
			this->textBox148->TabIndex = 184;
			this->textBox148->Text = S"00000-00000";
			// 
			// label153
			// 
			this->label153->Location = System::Drawing::Point(8, 40);
			this->label153->Name = S"label153";
			this->label153->Size = System::Drawing::Size(72, 24);
			this->label153->TabIndex = 183;
			this->label153->Text = S"LATITUDE";
			this->label153->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox147
			// 
			this->textBox147->Enabled = false;
			this->textBox147->Location = System::Drawing::Point(80, 16);
			this->textBox147->Name = S"textBox147";
			this->textBox147->Size = System::Drawing::Size(72, 20);
			this->textBox147->TabIndex = 182;
			this->textBox147->Text = S"00000-00000";
			// 
			// label152
			// 
			this->label152->Location = System::Drawing::Point(8, 16);
			this->label152->Name = S"label152";
			this->label152->Size = System::Drawing::Size(56, 24);
			this->label152->TabIndex = 181;
			this->label152->Text = S"RTHETA";
			this->label152->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// groupBox25
			// 
			this->groupBox25->Controls->Add(this->textBox154);
			this->groupBox25->Controls->Add(this->label159);
			this->groupBox25->Controls->Add(this->label160);
			this->groupBox25->Controls->Add(this->textBox155);
			this->groupBox25->Controls->Add(this->label161);
			this->groupBox25->Controls->Add(this->textBox156);
			this->groupBox25->Controls->Add(this->label162);
			this->groupBox25->Controls->Add(this->textBox157);
			this->groupBox25->Location = System::Drawing::Point(752, 576);
			this->groupBox25->Name = S"groupBox25";
			this->groupBox25->Size = System::Drawing::Size(96, 120);
			this->groupBox25->TabIndex = 219;
			this->groupBox25->TabStop = false;
			this->groupBox25->Text = S"SPS";
			// 
			// textBox154
			// 
			this->textBox154->Enabled = false;
			this->textBox154->Location = System::Drawing::Point(48, 64);
			this->textBox154->Name = S"textBox154";
			this->textBox154->Size = System::Drawing::Size(40, 20);
			this->textBox154->TabIndex = 151;
			this->textBox154->Text = S"00000";
			// 
			// label159
			// 
			this->label159->Location = System::Drawing::Point(8, 16);
			this->label159->Name = S"label159";
			this->label159->Size = System::Drawing::Size(40, 24);
			this->label159->TabIndex = 146;
			this->label159->Text = S"PTRIM";
			this->label159->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label160
			// 
			this->label160->Location = System::Drawing::Point(8, 40);
			this->label160->Name = S"label160";
			this->label160->Size = System::Drawing::Size(40, 24);
			this->label160->TabIndex = 147;
			this->label160->Text = S"YTRIM";
			this->label160->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox155
			// 
			this->textBox155->Enabled = false;
			this->textBox155->Location = System::Drawing::Point(48, 40);
			this->textBox155->Name = S"textBox155";
			this->textBox155->Size = System::Drawing::Size(40, 20);
			this->textBox155->TabIndex = 150;
			this->textBox155->Text = S"00000";
			// 
			// label161
			// 
			this->label161->Location = System::Drawing::Point(8, 64);
			this->label161->Name = S"label161";
			this->label161->Size = System::Drawing::Size(40, 24);
			this->label161->TabIndex = 148;
			this->label161->Text = S"PCMD";
			this->label161->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox156
			// 
			this->textBox156->Enabled = false;
			this->textBox156->Location = System::Drawing::Point(48, 16);
			this->textBox156->Name = S"textBox156";
			this->textBox156->Size = System::Drawing::Size(40, 20);
			this->textBox156->TabIndex = 149;
			this->textBox156->Text = S"00000";
			// 
			// label162
			// 
			this->label162->Location = System::Drawing::Point(8, 88);
			this->label162->Name = S"label162";
			this->label162->Size = System::Drawing::Size(40, 24);
			this->label162->TabIndex = 220;
			this->label162->Text = S"YCMD";
			this->label162->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox157
			// 
			this->textBox157->Enabled = false;
			this->textBox157->Location = System::Drawing::Point(48, 88);
			this->textBox157->Name = S"textBox157";
			this->textBox157->Size = System::Drawing::Size(40, 20);
			this->textBox157->TabIndex = 221;
			this->textBox157->Text = S"00000";
			// 
			// groupBox26
			// 
			this->groupBox26->Controls->Add(this->textBox161);
			this->groupBox26->Controls->Add(this->label166);
			this->groupBox26->Controls->Add(this->label167);
			this->groupBox26->Controls->Add(this->textBox162);
			this->groupBox26->Controls->Add(this->label168);
			this->groupBox26->Controls->Add(this->textBox163);
			this->groupBox26->Location = System::Drawing::Point(752, 480);
			this->groupBox26->Name = S"groupBox26";
			this->groupBox26->Size = System::Drawing::Size(96, 96);
			this->groupBox26->TabIndex = 220;
			this->groupBox26->TabStop = false;
			this->groupBox26->Text = S"ANGULAR RT";
			// 
			// textBox161
			// 
			this->textBox161->Enabled = false;
			this->textBox161->Location = System::Drawing::Point(48, 64);
			this->textBox161->Name = S"textBox161";
			this->textBox161->Size = System::Drawing::Size(40, 20);
			this->textBox161->TabIndex = 151;
			this->textBox161->Text = S"00000";
			// 
			// label166
			// 
			this->label166->Location = System::Drawing::Point(8, 16);
			this->label166->Name = S"label166";
			this->label166->Size = System::Drawing::Size(40, 24);
			this->label166->TabIndex = 146;
			this->label166->Text = S"ROLL";
			this->label166->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label167
			// 
			this->label167->Location = System::Drawing::Point(8, 40);
			this->label167->Name = S"label167";
			this->label167->Size = System::Drawing::Size(40, 24);
			this->label167->TabIndex = 147;
			this->label167->Text = S"PITCH";
			this->label167->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox162
			// 
			this->textBox162->Enabled = false;
			this->textBox162->Location = System::Drawing::Point(48, 40);
			this->textBox162->Name = S"textBox162";
			this->textBox162->Size = System::Drawing::Size(40, 20);
			this->textBox162->TabIndex = 150;
			this->textBox162->Text = S"00000";
			// 
			// label168
			// 
			this->label168->Location = System::Drawing::Point(8, 64);
			this->label168->Name = S"label168";
			this->label168->Size = System::Drawing::Size(40, 24);
			this->label168->TabIndex = 148;
			this->label168->Text = S"YAW";
			this->label168->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox163
			// 
			this->textBox163->Enabled = false;
			this->textBox163->Location = System::Drawing::Point(48, 16);
			this->textBox163->Name = S"textBox163";
			this->textBox163->Size = System::Drawing::Size(40, 20);
			this->textBox163->TabIndex = 149;
			this->textBox163->Text = S"00000";
			// 
			// groupBox27
			// 
			this->groupBox27->Controls->Add(this->textBox172);
			this->groupBox27->Controls->Add(this->label177);
			this->groupBox27->Controls->Add(this->label178);
			this->groupBox27->Controls->Add(this->textBox173);
			this->groupBox27->Controls->Add(this->label179);
			this->groupBox27->Controls->Add(this->textBox174);
			this->groupBox27->Location = System::Drawing::Point(264, 264);
			this->groupBox27->Name = S"groupBox27";
			this->groupBox27->Size = System::Drawing::Size(104, 96);
			this->groupBox27->TabIndex = 221;
			this->groupBox27->TabStop = false;
			this->groupBox27->Text = S"DELV PIPA";
			// 
			// textBox172
			// 
			this->textBox172->Enabled = false;
			this->textBox172->Location = System::Drawing::Point(24, 64);
			this->textBox172->Name = S"textBox172";
			this->textBox172->Size = System::Drawing::Size(72, 20);
			this->textBox172->TabIndex = 151;
			this->textBox172->Text = S"00000-00000";
			// 
			// label177
			// 
			this->label177->Location = System::Drawing::Point(8, 16);
			this->label177->Name = S"label177";
			this->label177->Size = System::Drawing::Size(16, 24);
			this->label177->TabIndex = 146;
			this->label177->Text = S"X";
			this->label177->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// label178
			// 
			this->label178->Location = System::Drawing::Point(8, 40);
			this->label178->Name = S"label178";
			this->label178->Size = System::Drawing::Size(16, 24);
			this->label178->TabIndex = 147;
			this->label178->Text = S"Y";
			this->label178->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox173
			// 
			this->textBox173->Enabled = false;
			this->textBox173->Location = System::Drawing::Point(24, 40);
			this->textBox173->Name = S"textBox173";
			this->textBox173->Size = System::Drawing::Size(72, 20);
			this->textBox173->TabIndex = 150;
			this->textBox173->Text = S"00000-00000";
			// 
			// label179
			// 
			this->label179->Location = System::Drawing::Point(8, 64);
			this->label179->Name = S"label179";
			this->label179->Size = System::Drawing::Size(16, 24);
			this->label179->TabIndex = 148;
			this->label179->Text = S"Z";
			this->label179->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox174
			// 
			this->textBox174->Enabled = false;
			this->textBox174->Location = System::Drawing::Point(24, 16);
			this->textBox174->Name = S"textBox174";
			this->textBox174->Size = System::Drawing::Size(72, 20);
			this->textBox174->TabIndex = 149;
			this->textBox174->Text = S"00000-00000";
			// 
			// CMCForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(1014, 700);
			this->Controls->Add(this->groupBox27);
			this->Controls->Add(this->groupBox26);
			this->Controls->Add(this->groupBox25);
			this->Controls->Add(this->groupBox24);
			this->Controls->Add(this->groupBox23);
			this->Controls->Add(this->groupBox22);
			this->Controls->Add(this->label78);
			this->Controls->Add(this->textBox72);
			this->Controls->Add(this->label67);
			this->Controls->Add(this->textBox71);
			this->Controls->Add(this->groupBox11);
			this->Controls->Add(this->groupBox20);
			this->Controls->Add(this->groupBox21);
			this->Controls->Add(this->groupBox19);
			this->Controls->Add(this->groupBox18);
			this->Controls->Add(this->label117);
			this->Controls->Add(this->textBox115);
			this->Controls->Add(this->groupBox17);
			this->Controls->Add(this->groupBox16);
			this->Controls->Add(this->label106);
			this->Controls->Add(this->textBox104);
			this->Controls->Add(this->label107);
			this->Controls->Add(this->textBox105);
			this->Controls->Add(this->label108);
			this->Controls->Add(this->textBox106);
			this->Controls->Add(this->label103);
			this->Controls->Add(this->textBox101);
			this->Controls->Add(this->label104);
			this->Controls->Add(this->textBox102);
			this->Controls->Add(this->groupBox15);
			this->Controls->Add(this->groupBox14);
			this->Controls->Add(this->groupBox13);
			this->Controls->Add(this->label90);
			this->Controls->Add(this->textBox88);
			this->Controls->Add(this->label91);
			this->Controls->Add(this->textBox89);
			this->Controls->Add(this->label92);
			this->Controls->Add(this->textBox90);
			this->Controls->Add(this->label88);
			this->Controls->Add(this->textBox86);
			this->Controls->Add(this->groupBox12);
			this->Controls->Add(this->label79);
			this->Controls->Add(this->textBox77);
			this->Controls->Add(this->label80);
			this->Controls->Add(this->textBox78);
			this->Controls->Add(this->groupBox10);
			this->Controls->Add(this->groupBox9);
			this->Controls->Add(this->label47);
			this->Controls->Add(this->textBox45);
			this->Controls->Add(this->label45);
			this->Controls->Add(this->textBox43);
			this->Controls->Add(this->label46);
			this->Controls->Add(this->textBox44);
			this->Controls->Add(this->label44);
			this->Controls->Add(this->textBox42);
			this->Controls->Add(this->groupBox8);
			this->Controls->Add(this->label36);
			this->Controls->Add(this->textBox34);
			this->Controls->Add(this->groupBox7);
			this->Controls->Add(this->label25);
			this->Controls->Add(this->textBox23);
			this->Controls->Add(this->groupBox6);
			this->Controls->Add(this->groupBox5);
			this->Controls->Add(this->groupBox4);
			this->Controls->Add(this->groupBox3);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->cmcListID);
			this->Controls->Add(this->label1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::Fixed3D;
			this->Name = S"CMCForm";
			this->Text = S"COMMAND MODULE COMPUTER STATUS";
			this->groupBox1->ResumeLayout(false);
			this->groupBox2->ResumeLayout(false);
			this->groupBox3->ResumeLayout(false);
			this->groupBox4->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->groupBox6->ResumeLayout(false);
			this->groupBox7->ResumeLayout(false);
			this->groupBox8->ResumeLayout(false);
			this->groupBox9->ResumeLayout(false);
			this->groupBox10->ResumeLayout(false);
			this->groupBox12->ResumeLayout(false);
			this->groupBox13->ResumeLayout(false);
			this->groupBox14->ResumeLayout(false);
			this->groupBox15->ResumeLayout(false);
			this->groupBox16->ResumeLayout(false);
			this->groupBox17->ResumeLayout(false);
			this->groupBox18->ResumeLayout(false);
			this->groupBox19->ResumeLayout(false);
			this->groupBox21->ResumeLayout(false);
			this->groupBox20->ResumeLayout(false);
			this->groupBox11->ResumeLayout(false);
			this->groupBox22->ResumeLayout(false);
			this->groupBox23->ResumeLayout(false);
			this->groupBox24->ResumeLayout(false);
			this->groupBox25->ResumeLayout(false);
			this->groupBox26->ResumeLayout(false);
			this->groupBox27->ResumeLayout(false);
			this->ResumeLayout(false);
		}				
};
}