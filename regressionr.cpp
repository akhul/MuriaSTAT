#include "regressionr.h"

RegressionR::RegressionR(RInside &m_R):R(m_R)
{

}

RegressionR::~RegressionR()
{

}
void RegressionR::setStatIndicator(QStringList stat){
    StatIndicator=stat;
}

QString RegressionR::getAllHtml(){
    return allHtml;
}
void RegressionR::setcoefHtml(){

    R.parseEvalQ("dn<-dimnames(model1$coefficients);"
                 "rowName<-dn[[1]];"
                 "colName<-dn[[2]]");

    //Rcpp::NumericVector coef = R.parseEval("model1$coefficients");
    Rcpp::CharacterVector rowName = R.parseEval("rowName");
    Rcpp::CharacterVector colName = R.parseEval("colName");
    //R.parseEvalQ("dimcoef<-as.vector(c(length(dn[[1]]),length(dn[[2]])))");

    //Rcpp::Vector dimCoef = R.parseEval("dimcoef");
    int row =rowName.length();
    int col =colName.length();

    R.parseEvalQ("coef<-matrix(model1$coefficient,ncol=length(colName),nrow=length(rowName));t_coef<-t(coef)");
    Rcpp::NumericMatrix coef=R.parseEval("t_coef");

    QString html="<p>Coefficient:</p><table align='left'>";

    int temp=0;
    int temp1=0;
    int temp2=0;
    for (int i=0; i<row+1; i++) {

        html.append("<tr>");

            for (int j=0; j<col+1; j++) {

                if(i==0&&j==0){
                    html.append("<td>");
                    html.append("</td>");

                }

                else if(i==0){
                    html.append("<th>");

                    html.append(QString(colName[temp1]));

                    html.append("</th>");
                    temp1=temp1+1;
                }
                else if(j==0){
                    html.append("<td>");

                    html.append(QString(rowName[temp2]));

                    html.append("</td>");
                    temp2=temp2+1;
                }
                else{

                    html.append("<td>");

                    html.append(QString::number(coef[temp]));

                    html.append("</td>");
                    temp=temp+1;


                }

            }
            html.append("</tr>");
        }
        html.append("</table>");
    coefHtml=html;

}

void RegressionR::setSummaryModel(){

    double rSquare = R.parseEval("model1$r.squared");
    double r =sqrt(rSquare);
    double adjR_Square =R.parseEval("model1$adj.r.squared");
    double sigma =R.parseEval("model1$sigma");

    QString html="<p>Summary Model:</p><table align='left'>"
            "<tr>"
                    "<th>Model</th>"
                    "<th>R</th>"
                    "<th>R-Square</th>"
                    "<th>Adjusted R-Square</th>"
                    "<th>Residual Standard Error</th>"

            "</tr>"
            "<tr>"
                    "<td>1</td>"
                    "<td>"+QString::number(r)+"</td>"
                    "<td>"+QString::number(rSquare)+"</td>"
                    "<td>"+QString::number(adjR_Square)+"</td>"
                    "<td>"+QString::number(sigma)+"</td>"

            "</tr></table>";
    summaryModelHtml=html;

}
void RegressionR::setResidualHtml(){
    Rcpp::NumericVector residual =R.parseEval("res<-quantile(model1$residuals)");


    QString html="<p>Residual:</p><table align='left'>"
            "<tr>"
                    "<th>Model</th>"
                    "<th>Min</th>"
                    "<th>1Q</th>"
                    "<th>Median</th>"
                    "<th>3Q</th>"
                    "<th>Max</th>"

            "</tr>"
            "<tr>"
                    "<td>1</td>";
            for (int i=0; i<residual.size(); i++){
                    html.append("<td>"+QString::number(residual[i])+"</td>");
            }
            html.append("</tr></table>");
    resHtml=html;

}

void RegressionR::setOuputHtml(std::string model,std::string datasetName){
    R.parseEvalQ("model1<-summary(lm("+model+","+datasetName+"))");
    if(StatIndicator.contains("model",Qt::CaseInsensitive)){
           setSummaryModel();
        }
    if(StatIndicator.contains("coefficients",Qt::CaseInsensitive)){
           setcoefHtml();
        }
    if(StatIndicator.contains("residual",Qt::CaseInsensitive)){
              setResidualHtml();
            }
   // if(StatIndicator.contains("anova",Qt::CaseInsensitive)){
          // setAnova();
     //   }
    QString tempHtml="<html><head>"
                                 "<style>"
                                     "table {"
                                             "color: #333;"
                                             "font-family: calibri;"
                                             "font-size: .8em;"
                                             "font-weight: 300;"
                                             "text-align: center;"
                                             "line-height: 27px;"
                                             "border-spacing: 1px ;"
                                             "border: 0px solid #428bca;"
                                             "margin: 2px auto;"
                                             "margin-left: 1px;"
                                             "margin-bottom: 20px;"
                                             "-moz-border-radius:5px;"
                                             "-webkit-border-radius:5px;"
                                             "border-radius:5px;"
                                             "border-top-left-radius:5px;"
                                             "}"
                                     "th, td {"
                                             "padding: 3px;"
                                             "background: #EBF0F5;"
                                             "}"
                                     "p{font-family: calibri;}"
                                     "td:first-child{"
                                             "width:100px ;"
                                             "background: #428bca;"
                                             "color: #fff;"
                                             "text-align: left;"
                                             "padding-left: 10px;"
                                             "}"
                                     "th{"
                                             "background: #428bca;"
                                             "color: #fff;"
                                             "}"
                                "</style>"
                             "</head>"
                             "<body>"+summaryModelHtml+coefHtml+resHtml+"</body></html>";
    allHtml=tempHtml;

}

