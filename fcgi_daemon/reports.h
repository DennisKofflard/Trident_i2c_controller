/*---------------------------------------------------------------------------
 *
 * Created by Dennis Kofflard, dennis@kofflard.com
 * 2015-12-11
 *
 *---------------------------------------------------------------------------
 */


#ifndef REPORTS_H
#define REPORTS_H


void terminal_clear();
void terminal_home();
int report_all_wrapper();
int report_all();
int psu_reports(i2c_dev *dev);


#endif /* REPORTS_H */
