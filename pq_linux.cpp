#include "pq_linux.h"
#include "ui_pq_linux.h"


#include "workthread.h"
#include "socket_send.h"
#include <stdio.h>
#include <math.h>
#include "V_Dip_Swell_Interrupt.h"
#include "main.h"

static QString tempStr;
pq_linux::pq_linux(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pq_linux)
{
    ui->setupUi(this);
    setWindowTitle(tr("电能质量分析"));
       timer1=new QTimer;
       connect(timer1,SIGNAL(timeout()),this,SLOT(update()));
       timer1->start(200);

       //
       data_view = new QStandardItemModel();
       ui->tableView->setModel(data_view);
       data_view->setColumnCount(2);
       data_view->setHeaderData(0, Qt::Horizontal, "频率");
       data_view->setHeaderData(1, Qt::Horizontal, "有效值");
       data_view->setRowCount(10);
       data_view->setHeaderData(0, Qt::Vertical, "0");

       data_view->setHeaderData(1, Qt::Vertical, "1");

       data_view->setHeaderData(2, Qt::Vertical, "2");
       data_view->setHeaderData(3, Qt::Vertical, "3");

       data_view->setHeaderData(4, Qt::Vertical, "4");

       data_view->setHeaderData(5, Qt::Vertical, "5");
       data_view->setHeaderData(6, Qt::Vertical, "6");

       data_view->setHeaderData(7, Qt::Vertical, "7");

       data_view->setHeaderData(8, Qt::Vertical, "8");
       data_view->setHeaderData(9, Qt::Vertical, "9");



           connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(shuaxin()));
           connect(ui->flicker_updateButton, SIGNAL(clicked()), this, SLOT(A_flicker_shuaxin()));
           connect(ui->B_flicker_updateButton, SIGNAL(clicked()), this, SLOT(B_flicker_shuaxin()));
           connect(ui->C_flicker_updateButton, SIGNAL(clicked()), this, SLOT(C_flicker_shuaxin()));
           connect(ui->fuwei, SIGNAL(clicked()), this, SLOT(fuwei()));
           connect(ui->cal_push, SIGNAL(clicked()), this, SLOT(calculate()));

           ui->threadsum->setText(tempStr.setNum(threadsum));
           ui->cpu_num->setText(tempStr.setNum(cpu_num));
           ui->System_Start_Time->setText(start_time_s);
}

pq_linux::~pq_linux()
{
    delete ui;
}

void pq_linux::calculate(void)
{
    char tmp[100];
    A_U_channel_index=ui->A_U_channel_index->text().toDouble();
    A_I_channel_index=ui->A_I_channel_index->text().toDouble();
    B_U_channel_index=ui->B_U_channel_index->text().toDouble();
    B_I_channel_index=ui->B_I_channel_index->text().toDouble();
    C_U_channel_index=ui->C_U_channel_index->text().toDouble();
    C_I_channel_index=ui->C_I_channel_index->text().toDouble();
    double real_value=ui->real_value->text().toDouble();
    double rated_value=ui->rated_value->text().toDouble();
    double cal_error=(A_rms-real_value)/rated_value*10000;
    sprintf(tmp, "%.5f", A_rms);
    ui->test_value->setText(tmp);
    sprintf(tmp, "%.5f", cal_error);
    ui->cal_error->setText(tmp);

}
int A_flicker_open, A_voltage_dipswellinterrupt_open;
int B_flicker_open, B_voltage_dipswellinterrupt_open;
int C_flicker_open, C_voltage_dipswellinterrupt_open;
int curve_open;
static time_t real_time;
static long totaltime;
static char total_time[100];
static int day, hour, minute, sec;

extern short output1,output2;
extern DList *list_f;
double f_array[10];
void pq_linux::update(void)
{
    double active_power_meter_tmp, reactive_power_meter_tmp;
    double B_active_power_meter_tmp, B_reactive_power_meter_tmp;
    double C_active_power_meter_tmp, C_reactive_power_meter_tmp;
    char tmp[100];

    A_flicker_open = ui->flicker_comboBox->currentIndex();
    B_flicker_open = ui->B_flicker_comboBox->currentIndex();
    C_flicker_open = ui->C_flicker_comboBox->currentIndex();
    A_voltage_dipswellinterrupt_open = ui->voltage_dipswellinterrupt_comboBox->currentIndex();
    B_voltage_dipswellinterrupt_open = ui->B_voltage_dipswellinterrupt_comboBox->currentIndex();
    C_voltage_dipswellinterrupt_open = ui->C_voltage_dipswellinterrupt_comboBox->currentIndex();


    ui->bind_flag->setText(tempStr.setNum(bind_flag));
    //测试接口
    ui->output0->setText(tempStr.setNum(count));
    sprintf(tmp, "%d", output1);
    ui->output1->setText(tmp);
    sprintf(tmp, "0x%x", output2);
    ui->output2->setText(tmp);

    {
        time(&real_time);
        totaltime = (real_time - start_time);

        day = totaltime / (60 * 60 * 24);
        totaltime = totaltime % (60 * 60 * 24);
        hour = totaltime / (60 * 60);
        totaltime = totaltime % (60 * 60);
        minute = totaltime / 60;
        totaltime = totaltime % 60;
        sec = totaltime;
        sprintf(total_time, "%2d日%2d时%2d分%2d秒\n", day, hour, minute, sec);//年月日时分秒
        ui->total_time->setText(total_time);
    }

    sprintf(tmp, "%.4f", A_result_800half);
    ui->result_800->setText(tmp);
    sprintf(tmp, "%.4f", A_result_400half);
    ui->result_400->setText(tmp);
    sprintf(tmp, "%.4f", B_result_800half);
    ui->B_result_800->setText(tmp);
    sprintf(tmp, "%.4f", B_result_400half);
    ui->B_result_400->setText(tmp);
    sprintf(tmp, "%.4f", C_result_800half);
    ui->C_result_800->setText(tmp);
    sprintf(tmp, "%.4f", C_result_400half);
    ui->C_result_400->setText(tmp);
    ui->A_FFT_flag->setText(tempStr.setNum(A_FFT));
    ui->B_FFT_flag->setText(tempStr.setNum(B_FFT));
    ui->C_FFT_flag->setText(tempStr.setNum(C_FFT));
    ui->index->setText(tempStr.setNum(an_buffer_idx));

    {

        //static double test_data = 50.0;
            //test_data = test_data + 0.01;
            //ChangeData(list_f, test_data);

                struct node *temp = list_f->head;
                for (int i = 0; i < list_f->len; i++)
                {
                    f_array[i] = temp->data;
                    temp = temp->next;
                }

                sprintf(tmp, "%.7f", f_array[0]);
                data_view->setItem(0, 0, new QStandardItem(tmp));
                sprintf(tmp, "%.7f", f_array[1]);
                data_view->setItem(1, 0, new QStandardItem(tmp));
                sprintf(tmp, "%.7f", f_array[2]);
                data_view->setItem(2, 0, new QStandardItem(tmp));
                sprintf(tmp, "%.7f", f_array[3]);
                data_view->setItem(3, 0, new QStandardItem(tmp));
                sprintf(tmp, "%.7f", f_array[4]);
                data_view->setItem(4, 0, new QStandardItem(tmp));
                sprintf(tmp, "%.7f", f_array[5]);
                data_view->setItem(5, 0, new QStandardItem(tmp));
                sprintf(tmp, "%.7f", f_array[6]);
                data_view->setItem(6, 0, new QStandardItem(tmp));
                sprintf(tmp, "%.7f", f_array[7]);
                data_view->setItem(7, 0, new QStandardItem(tmp));
                sprintf(tmp, "%.7f", f_array[8]);
                data_view->setItem(8, 0, new QStandardItem(tmp));
                sprintf(tmp, "%.7f", f_array[9]);
                data_view->setItem(9, 0, new QStandardItem(tmp));

                data_view->setItem(0, 1, new QStandardItem("3"));

    }

    //
    {
        sprintf(tmp, "%.3f", fftw_phase_a_vol[1] / PI * 180);
        ui->A_jiboxiangwei->setText(tmp);
        sprintf(tmp, "%.3f", fftw_phase_b[1] / PI * 180);
        ui->B_jiboxiangwei->setText(tmp);
        sprintf(tmp, "%.3f", fftw_phase_c[1] / PI * 180);
        ui->C_jiboxiangwei->setText(tmp);
        sprintf(tmp, "%.4f", BA_phase_average / PI * 180);
        ui->BA_xiangweicha->setText(tmp);
        sprintf(tmp, "%.4f", CA_phase_average / PI * 180);
        ui->CA_xiangweicha->setText(tmp);
    }
    ui->A_flag->setText(tempStr.setNum(A_flag));
    ui->B_flag->setText(tempStr.setNum(B_flag));
    ui->C_flag->setText(tempStr.setNum(C_flag));

    //A
    {
        //A
        sprintf(tmp, "%.7f", A_rms);
        ui->V_rms->setText(tmp);
        //A
        sprintf(tmp, "%.7f", A_cur_rms);
        ui->I_rms->setText(tmp);
        //
        sprintf(tmp, "%.7f", A_fre);
        ui->FHz->setText(tmp);
        //
        sprintf(tmp, "%.7f", A_active_power);
        ui->PW->setText(tmp);
        //
        active_power_meter_tmp = A_active_power_meter;
        sprintf(tmp, "%.9f", active_power_meter_tmp);
        ui->PT->setText(tmp);
        //
        sprintf(tmp, "%.7f", A_reactive_power);
        ui->QVar->setText(tmp);
        //
        reactive_power_meter_tmp = A_reactive_power_meter;
        sprintf(tmp, "%.9f", reactive_power_meter_tmp);
        ui->QT->setText(tmp);
        //
        sprintf(tmp, "%.7f", A_apparent_power);
        ui->SVA->setText(tmp);

        //THDU
        sprintf(tmp, "%.5f%%", THDU);
        ui->Thdu->setText(tmp);
        //
        sprintf(tmp, "%.6f", fftw_phase_differ[1] / PI * 180);
        ui->VI_xiangweicha->setText(tmp);
        //
        sprintf(tmp, "%.6f", cos(fftw_phase_differ[1]));
        ui->VI_cos->setText(tmp);
        //
        sprintf(tmp, "%.9f", fuzhi_a[1]);
        ui->V_jibofuzhi->setText(tmp);
        //
        sprintf(tmp, "%.9f", fuzhi_a_cur[1]);
        ui->I_jibofuzhi->setText(tmp);
        //
        sprintf(tmp, "%.9f", fuzhi_a[1] / fuzhi_a_cur[1]);
        ui->VI_jibofuzhibi->setText(tmp);
    }
    //
    {
        sprintf(tmp, "%.8f", fuzhi_a[0]);
        ui->zhiliufenliang->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[2]);
        ui->ercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[3]);
        ui->sancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[4]);
        ui->sicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[5]);
        ui->wucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[6]);
        ui->liucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[7]);
        ui->qicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[8]);
        ui->bacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[9]);
        ui->jiucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[10]);
        ui->shicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[11]);
        ui->shiyicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[12]);
        ui->shiercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[13]);
        ui->shisancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[14]);
        ui->shisicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[15]);
        ui->shiwucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[16]);
        ui->shiliucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[17]);
        ui->shiqicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[18]);
        ui->shibacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[19]);
        ui->shijiucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[20]);
        ui->ershicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[21]);
        ui->ershiyicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[22]);
        ui->ershiercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[23]);
        ui->ershisancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[24]);
        ui->ershisicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[25]);
        ui->ershiwucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[26]);
        ui->ershiliucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[27]);
        ui->ershiqicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[28]);
        ui->ershibacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[29]);
        ui->ershijiucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[30]);
        ui->sanshicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[31]);
        ui->sanshiyicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[32]);
        ui->sanshiercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[33]);
        ui->sanshisancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[34]);
        ui->sanshisicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[35]);
        ui->sanshiwucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[36]);
        ui->sanshiliucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[37]);
        ui->sanshiqicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[38]);
        ui->sanshibacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_a[39]);
        ui->sanshijiucixiebofuzhi->setText(tmp);
    }

    {

        sprintf(tmp, "%.7f", B_rms);
        ui->B_rms->setText(tmp);
        sprintf(tmp, "%.7f", B_cur_rms);
        ui->B_I_rms->setText(tmp);
        sprintf(tmp, "%.7f", B_fre);
        ui->B_FHz->setText(tmp);
        sprintf(tmp, "%.7f", B_active_power);
        ui->B_PW->setText(tmp);
        B_active_power_meter_tmp = B_active_power_meter;
        sprintf(tmp, "%.9f", B_active_power_meter_tmp);
        ui->B_PT->setText(tmp);
        sprintf(tmp, "%.7f", B_reactive_power);
        ui->B_QVar->setText(tmp);
        B_reactive_power_meter_tmp = B_reactive_power_meter;
        sprintf(tmp, "%.9f", B_reactive_power_meter_tmp);
        ui->B_QT->setText(tmp);
        sprintf(tmp, "%.7f", B_apparent_power);
        ui->B_SVA->setText(tmp);

        sprintf(tmp, "%.5f%%", B_THDU);
        ui->B_Thdu->setText(tmp);
        sprintf(tmp, "%.6f", fftw_phase_differ_b[1] / PI * 180);
        ui->B_VI_xiangweicha->setText(tmp);
        sprintf(tmp, "%.6f", cos(fftw_phase_differ_b[1]));
        ui->B_VI_cos->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[1]);
        ui->B_V_jibofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b_cur[1]);
        ui->B_I_jibofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[1] / fuzhi_b_cur[1]);
        ui->B_VI_jibofuzhibi->setText(tmp);

    }
    {
        sprintf(tmp, "%.8f", fuzhi_b[0]);
        ui->B_zhiliufenliang->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[1]);
        ui->B_V_jibofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[2]);
        ui->B_ercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[3]);
        ui->B_sancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[4]);
        ui->B_sicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[5]);
        ui->B_wucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[6]);
        ui->B_liucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[7]);
        ui->B_qicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[8]);
        ui->B_bacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[9]);
        ui->B_jiucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[10]);
        ui->B_shicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[11]);
        ui->B_shiyicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[12]);
        ui->B_shiercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[13]);
        ui->B_shisancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[14]);
        ui->B_shisicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[15]);
        ui->B_shiwucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[16]);
        ui->B_shiliucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[17]);
        ui->B_shiqicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[18]);
        ui->B_shibacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[19]);
        ui->B_shijiucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[20]);
        ui->B_ershicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[21]);
        ui->B_ershiyicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[22]);
        ui->B_ershiercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[23]);
        ui->B_ershisancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[24]);
        ui->B_ershisicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[25]);
        ui->B_ershiwucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[26]);
        ui->B_ershiliucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[27]);
        ui->B_ershiqicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[28]);
        ui->B_ershibacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[29]);
        ui->B_ershijiucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[30]);
        ui->B_sanshicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[31]);
        ui->B_sanshiyicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[32]);
        ui->B_sanshiercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[33]);
        ui->B_sanshisancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[34]);
        ui->B_sanshisicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[35]);
        ui->B_sanshiwucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[36]);
        ui->B_sanshiliucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[37]);
        ui->B_sanshiqicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[38]);
        ui->B_sanshibacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_b[39]);
        ui->B_sanshijiucixiebofuzhi->setText(tmp);
    }
    {
        sprintf(tmp, "%.7f", C_rms);
        ui->C_rms->setText(tmp);
        sprintf(tmp, "%.7f", C_cur_rms);
        ui->C_I_rms->setText(tmp);
        sprintf(tmp, "%.7f", C_fre);
        ui->C_FHz->setText(tmp);
        sprintf(tmp, "%.7f", C_active_power);
        ui->C_PW->setText(tmp);
        C_active_power_meter_tmp = C_active_power_meter;
        sprintf(tmp, "%.9f", C_active_power_meter_tmp);
        ui->C_PT->setText(tmp);
        sprintf(tmp, "%.7f", C_reactive_power);
        ui->C_QVar->setText(tmp);
        C_reactive_power_meter_tmp = C_reactive_power_meter;
        sprintf(tmp, "%.9f", C_reactive_power_meter_tmp);
        ui->C_QT->setText(tmp);
        sprintf(tmp, "%.7f", C_apparent_power);
        ui->C_SVA->setText(tmp);
        sprintf(tmp, "%.5f%%", C_THDU);
        ui->C_Thdu->setText(tmp);
        sprintf(tmp, "%.6f", fftw_phase_differ_c[1] / PI * 180);
        ui->C_VI_xiangweicha->setText(tmp);
        sprintf(tmp, "%.6f", cos(fftw_phase_differ_c[1]));
        ui->C_VI_cos->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[1]);
        ui->C_V_jibofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c_cur[1]);
        ui->C_I_jibofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[1] / fuzhi_c_cur[1]);
        ui->C_VI_jibofuzhibi->setText(tmp);
    }
    {
        sprintf(tmp, "%.8f", fuzhi_c[0]);
        ui->C_zhiliufenliang->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[1]);
        ui->C_V_jibofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[2]);
        ui->C_ercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[3]);
        ui->C_sancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[4]);
        ui->C_sicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[5]);
        ui->C_wucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[6]);
        ui->C_liucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[7]);
        ui->C_qicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[8]);
        ui->C_bacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[9]);
        ui->C_jiucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[10]);
        ui->C_shicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[11]);
        ui->C_shiyicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[12]);
        ui->C_shiercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[13]);
        ui->C_shisancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[14]);
        ui->C_shisicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[15]);
        ui->C_shiwucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[16]);
        ui->C_shiliucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[17]);
        ui->C_shiqicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[18]);
        ui->C_shibacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[19]);
        ui->C_shijiucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[20]);
        ui->C_ershicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[21]);
        ui->C_ershiyicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[22]);
        ui->C_ershiercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[23]);
        ui->C_ershisancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[24]);
        ui->C_ershisicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[25]);
        ui->C_ershiwucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[26]);
        ui->C_ershiliucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[27]);
        ui->C_ershiqicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[28]);
        ui->C_ershibacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[29]);
        ui->C_ershijiucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[30]);
        ui->C_sanshicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[31]);
        ui->C_sanshiyicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[32]);
        ui->C_sanshiercixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[33]);
        ui->C_sanshisancixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[34]);
        ui->C_sanshisicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[35]);
        ui->C_sanshiwucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[36]);
        ui->C_sanshiliucixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[37]);
        ui->C_sanshiqicixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[38]);
        ui->C_sanshibacixiebofuzhi->setText(tmp);
        sprintf(tmp, "%.9f", fuzhi_c[39]);
        ui->C_sanshijiucixiebofuzhi->setText(tmp);
    }
    //if ((A_VoltagedipDurationLastTime > 20)||(A_VoltagedipDurationLastTime==0))
    {
        ui->V_Dip_StartTime->setText(A_dip);
        ui->V_Dip_Flag->setText(tempStr.setNum(A_voltagedipstartflag));
        sprintf(tmp, "%.6f", A_VoltagedipDepth);
        ui->V_Dip_Depth->setText(tmp);
        sprintf(tmp, "%.6f", A_VoltagedipLastVoltageResult);
        ui->Last_V_Dip_Depth->setText(tmp);
        ui->V_Dip_Time->setText(tempStr.setNum(A_VoltagedipDurationTime));
        ui->V_Dip_LastTime->setText(tempStr.setNum(A_VoltagedipDurationLastTime));
    }
    //if ((A_VoltageswellDurationLastTime > 20) || (A_VoltageswellDurationLastTime == 0))
    {
        ui->V_Swell_StartTime->setText(A_swell);
        ui->V_Swell_Flag->setText(tempStr.setNum(A_voltageswellstartflag));
        sprintf(tmp, "%.6f", A_VoltageswellVoltageResult);
        ui->V_Swell->setText(tmp);
        sprintf(tmp, "%.6f", A_VoltageswellLastVoltageResult);
        ui->Last_V_Swell->setText(tmp);
        ui->V_Swell_Time->setText(tempStr.setNum(A_VoltageswellDurationTime));
        ui->V_Swell_LastTime->setText(tempStr.setNum(A_VoltageswellDurationLastTime));
    }
    //if ((A_VoltageinterruptionDurationLastTime > 20) || (A_VoltageinterruptionDurationLastTime == 0))
    {
        ui->V_Interrupt_StartTime->setText(A_interrupt);
        ui->V_Interrupt_Flag->setText(tempStr.setNum(A_voltageinterruptstartflag));
        sprintf(tmp, "%.6f", A_VoltageinterruptVoltageResult);
        ui->V_Interrupt->setText(tmp);
        sprintf(tmp, "%.6f", A_VoltageinterruptLastVoltageResult);
        ui->Last_V_Interrupt->setText(tmp);
        ui->V_Interrupt_Time->setText(tempStr.setNum(A_VoltageinterruptionDurationTime));
        ui->V_Interrupt_LasttTime->setText(tempStr.setNum(A_VoltageinterruptionDurationLastTime));
    }
//if ((B_VoltagedipDurationLastTime > 20)||(B_VoltagedipDurationLastTime==0))
    {
        ui->B_V_Dip_StartTime->setText(B_dip);
        ui->B_V_Dip_Flag->setText(tempStr.setNum(B_voltagedipstartflag));
        sprintf(tmp, "%.6f", B_VoltagedipDepth);
        ui->B_V_Dip_Depth->setText(tmp);
        sprintf(tmp, "%.6f", B_VoltagedipLastVoltageResult);
        ui->B_Last_V_Dip_Depth->setText(tmp);
        ui->B_V_Dip_Time->setText(tempStr.setNum(B_VoltagedipDurationTime));
        ui->B_V_Dip_LastTime->setText(tempStr.setNum(B_VoltagedipDurationLastTime));
    }
    //if ((B_VoltageswellDurationLastTime > 20) || (B_VoltageswellDurationLastTime == 0))
    {
        ui->B_V_Swell_StartTime->setText(B_swell);
        ui->B_V_Swell_Flag->setText(tempStr.setNum(B_voltageswellstartflag));
        sprintf(tmp, "%.6f", B_VoltageswellVoltageResult);
        ui->B_V_Swell->setText(tmp);
        sprintf(tmp, "%.6f", B_VoltageswellLastVoltageResult);
        ui->B_Last_V_Swell->setText(tmp);
        ui->B_V_Swell_Time->setText(tempStr.setNum(B_VoltageswellDurationTime));
        ui->B_V_Swell_LastTime->setText(tempStr.setNum(B_VoltageswellDurationLastTime));
    }
    //if ((B_VoltageinterruptionDurationLastTime > 20) || (B_VoltageinterruptionDurationLastTime == 0))
    {
        ui->B_V_Interrupt_StartTime->setText(B_interrupt);
        ui->B_V_Interrupt_Flag->setText(tempStr.setNum(B_voltageinterruptstartflag));
        sprintf(tmp, "%.6f", B_VoltageinterruptVoltageResult);
        ui->B_V_Interrupt->setText(tmp);
        sprintf(tmp, "%.6f", B_VoltageinterruptLastVoltageResult);
        ui->B_Last_V_Interrupt->setText(tmp);
        ui->B_V_Interrupt_Time->setText(tempStr.setNum(B_VoltageinterruptionDurationTime));
        ui->B_V_Interrupt_LasttTime->setText(tempStr.setNum(B_VoltageinterruptionDurationLastTime));
    }
//if ((C_VoltagedipDurationLastTime > 20)||(C_VoltagedipDurationLastTime==0))
    {
        ui->C_V_Dip_StartTime->setText(C_dip);
        ui->C_V_Dip_Flag->setText(tempStr.setNum(C_voltagedipstartflag));
        sprintf(tmp, "%.6f", C_VoltagedipDepth);
        ui->C_V_Dip_Depth->setText(tmp);
        sprintf(tmp, "%.6f", C_VoltagedipLastVoltageResult);
        ui->C_Last_V_Dip_Depth->setText(tmp);
        ui->C_V_Dip_Time->setText(tempStr.setNum(C_VoltagedipDurationTime));
        ui->C_V_Dip_LastTime->setText(tempStr.setNum(C_VoltagedipDurationLastTime));
    }
    //if ((C_VoltageswellDurationLastTime > 20) || (C_VoltageswellDurationLastTime == 0))
    {
        ui->C_V_Swell_StartTime->setText(C_swell);
        ui->C_V_Swell_Flag->setText(tempStr.setNum(C_voltageswellstartflag));
        sprintf(tmp, "%.6f", C_VoltageswellVoltageResult);
        ui->C_V_Swell->setText(tmp);
        sprintf(tmp, "%.6f", C_VoltageswellLastVoltageResult);
        ui->C_Last_V_Swell->setText(tmp);
        ui->C_V_Swell_Time->setText(tempStr.setNum(C_VoltageswellDurationTime));
        ui->C_V_Swell_LastTime->setText(tempStr.setNum(C_VoltageswellDurationLastTime));
    }
    //if ((C_VoltageinterruptionDurationLastTime > 20) || (C_VoltageinterruptionDurationLastTime == 0))
    {
        ui->C_V_Interrupt_StartTime->setText(C_interrupt);
        ui->C_V_Interrupt_Flag->setText(tempStr.setNum(C_voltageinterruptstartflag));
        sprintf(tmp, "%.6f", C_VoltageinterruptVoltageResult);
        ui->C_V_Interrupt->setText(tmp);
        sprintf(tmp, "%.6f", C_VoltageinterruptLastVoltageResult);
        ui->C_Last_V_Interrupt->setText(tmp);
        ui->C_V_Interrupt_Time->setText(tempStr.setNum(C_VoltageinterruptionDurationTime));
        ui->C_V_Interrupt_LasttTime->setText(tempStr.setNum(C_VoltageinterruptionDurationLastTime));
    }
    sprintf(tmp, "%.9f", uneg_param1);
    ui->uneg_param1->setText(tmp);
    sprintf(tmp, "%.9f", uneg_param2);
    ui->uneg_param2->setText(tmp);
    sprintf(tmp, "%.3f%%", uneg * 100);
    ui->sanxiangdianyabupinghengdu->setText(tmp);
    {
        sprintf(tmp, "%.1f", A_tiaozhibo_f);
        ui->A_tiaozhibo_f->setText(tmp);
        sprintf(tmp, "%.5f%%", A_V_fluctuation * 100);
        ui->tiaozhibo_xishu->setText(tmp);
        ui->shanbianjisuancishu->setText(tempStr.setNum(A_shanbianCount));
        sprintf(tmp, "%.7f", A_InstantaneousFlickerValue);
        ui->shunshishanbianzhi->setText(tmp);
        sprintf(tmp, "%.7f", A_ShorttimeFlickerValue);
        ui->duanshishanbianzhi->setText(tmp);
        sprintf(tmp, "%.7f", A_LongtimeFlickerValue);
        ui->changshishanbianzhi->setText(tmp);
    }
    {
        sprintf(tmp, "%.1f", B_tiaozhibo_f);
        ui->B_tiaozhibo_f->setText(tmp);
        sprintf(tmp, "%.5f%%", B_V_fluctuation * 100);
        ui->B_tiaozhibo_xishu->setText(tmp);
        ui->B_shanbianjisuancishu->setText(tempStr.setNum(B_shanbianCount));
        sprintf(tmp, "%.7f", B_InstantaneousFlickerValue);
        ui->B_shunshishanbianzhi->setText(tmp);
        sprintf(tmp, "%.7f", B_ShorttimeFlickerValue);
        ui->B_duanshishanbianzhi->setText(tmp);
        sprintf(tmp, "%.7f", B_LongtimeFlickerValue);
        ui->B_changshishanbianzhi->setText(tmp);
    }
    {
        sprintf(tmp, "%.1f", C_tiaozhibo_f);
        ui->C_tiaozhibo_f->setText(tmp);
        sprintf(tmp, "%.5f%%", C_V_fluctuation * 100);
        ui->C_tiaozhibo_xishu->setText(tmp);
        ui->C_shanbianjisuancishu->setText(tempStr.setNum(C_shanbianCount));
        sprintf(tmp, "%.7f", C_InstantaneousFlickerValue);
        ui->C_shunshishanbianzhi->setText(tmp);
        sprintf(tmp, "%.7f", C_ShorttimeFlickerValue);
        ui->C_duanshishanbianzhi->setText(tmp);
        sprintf(tmp, "%.7f", C_LongtimeFlickerValue);
        ui->C_changshishanbianzhi->setText(tmp);
    }

    ui->receivecount->setText(tempStr.setNum(packet_number));
    {
        ui->loss->setText(tempStr.setNum(A_err_flag));
        ui->loss_current->setText(tempStr.setNum(A_err_current));
        ui->loss_sum->setText(tempStr.setNum(A_err_sum));
    }
    ui->point_count->setText(tempStr.setNum(pointfre));
    ui->FFT_Number->setText(tempStr.setNum(A_FFT_Number));

    packet_number = 0;
}
extern char output_flag;
extern char send_flag_temp;
void pq_linux::shuaxin(void)
{
        memset(A_dip, 0, sizeof(A_dip));
        A_VoltagedipLastVoltageResult = 0;
        A_VoltagedipDurationLastTime = 0;
        memset(A_swell, 0, sizeof(A_swell));
        A_VoltageswellLastVoltageResult = 0;
        A_VoltageswellDurationLastTime = 0;
        memset(A_interrupt, 0, sizeof(A_interrupt));
        A_VoltageinterruptLastVoltageResult = 0;
        A_VoltageinterruptionDurationLastTime = 0;

        memset(B_dip, 0, sizeof(B_dip));
        B_VoltagedipLastVoltageResult = 0;
        B_VoltagedipDurationLastTime = 0;
        memset(B_swell, 0, sizeof(B_swell));
        B_VoltageswellLastVoltageResult = 0;
        B_VoltageswellDurationLastTime = 0;
        memset(B_interrupt, 0, sizeof(B_interrupt));
        B_VoltageinterruptLastVoltageResult = 0;
        B_VoltageinterruptionDurationLastTime = 0;

        memset(C_dip, 0, sizeof(C_dip));
        C_VoltagedipLastVoltageResult = 0;
        C_VoltagedipDurationLastTime = 0;
        memset(C_swell, 0, sizeof(C_swell));
        C_VoltageswellLastVoltageResult = 0;
        C_VoltageswellDurationLastTime = 0;
        memset(C_interrupt, 0, sizeof(C_interrupt));
        C_VoltageinterruptLastVoltageResult = 0;
        C_VoltageinterruptionDurationLastTime = 0;

        A_err_flag = 0;
        B_err_flag = 0;
        C_err_flag = 0;
        A_err_current = 0;
        B_err_current = 0;
        C_err_current = 0;
        A_err_sum = 0;
        B_err_sum = 0;
        C_err_sum = 0;
        output_flag=0;
        send_flag_temp=0;
}

void pq_linux::A_flicker_shuaxin(void)
{
        A_instantaneousflickervaluecnt = 0;
        A_shanbianCount = 0;
        A_tiaozhibo_f = 0;
        A_V_fluctuation = 0;
        A_InstantaneousFlickerValue = 0;
        A_ShorttimeFlickerValue = 0;
        A_LongtimeFlickerValue = 0;
}
void pq_linux::B_flicker_shuaxin(void)
{
        B_instantaneousflickervaluecnt = 0;
        B_shanbianCount = 0;
        B_tiaozhibo_f = 0;
        B_V_fluctuation = 0;
        B_InstantaneousFlickerValue = 0;
        B_ShorttimeFlickerValue = 0;
        B_LongtimeFlickerValue = 0;
}
void pq_linux::C_flicker_shuaxin(void)
{
        C_instantaneousflickervaluecnt = 0;
        C_shanbianCount = 0;
        C_tiaozhibo_f = 0;
        C_V_fluctuation = 0;
        C_InstantaneousFlickerValue = 0;
        C_ShorttimeFlickerValue = 0;
        C_LongtimeFlickerValue = 0;
}
void pq_linux::fuwei(void)
{
        A_fre = 0;
        B_fre = 0;
        C_fre = 0;

        A_result_800half = 0;
        A_result_400half = 0;
        A_FFT = 0;
        B_FFT = 0;
        C_FFT = 0;
        an_buffer_idx = 0;
        fftw_phase_a_vol[1] = 0;
        fftw_phase_b[1] = 0;
        fftw_phase_c[1] = 0;
        BA_phase_average = 0;
        CA_phase_average = 0;
        A_flag = 0;
        B_flag = 0;
        C_flag = 0;
        A_rms = 0;
        A_cur_rms = 0;
        A_active_power = 0;
        A_reactive_power = 0;
        A_apparent_power = 0;
        THDU = 0;
        fftw_phase_differ[1] = 0;
        fuzhi_a[1] = 0;
        fuzhi_a_cur[1] = 0;
        fuzhi_a[0] = 0;
        B_rms = 0;
        B_cur_rms = 0;
        B_active_power = 0;
        B_reactive_power = 0;
        B_apparent_power = 0;
        B_THDU = 0;
        fftw_phase_differ_b[1] = 0;
        fuzhi_b[1] = 0;
        fuzhi_b_cur[1] = 0;
        fuzhi_b[0] = 0;
        fuzhi_b[1] = 0;
        C_rms = 0;
        C_cur_rms = 0;
        C_active_power = 0;
        C_reactive_power = 0;
        C_apparent_power = 0;
        C_THDU = 0;
        fftw_phase_differ_c[1] = 0;
        fuzhi_c[1] = 0;
        fuzhi_c_cur[1] = 0;
        fuzhi_c[0] = 0;
        fuzhi_c[1] = 0;
        uneg_param1 = 0;
        uneg_param2 = 0;
        uneg = 0;
        packet_number = 0;
        pointfre = 0;
        A_FFT_Number = 0;
        C_reactive_power = 0;
        C_reactive_power = 0;
}
