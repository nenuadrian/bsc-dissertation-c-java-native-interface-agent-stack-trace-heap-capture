package com.insider.providers

import courier._, Defaults._
import javax.mail.internet.InternetAddress
import com.typesafe.config.Config

class EmailProvider(val config: Config) {
  val mailer = Mailer(config.getString("smtp.host"), config.getInt("smtp.port"))
    .auth(true)
    .as(config.getString("smtp.username"), config.getString("smtp.password"))
    .startTtls(false)()

  def test() = {
    s"""
    """
  }

  def sendEmail(subject: String, content: String, toEmail: String) {
    mailer(Envelope.from(new InternetAddress( config.getString("smtp.from"), config.getString("smtp.from-name") ))
        .to(new InternetAddress( toEmail ))
      //  .cc("dad" `@` "gmail.com")
        .subject(subject)
        /*content(Multipart()
           .attach(new java.io.File("tps.xls"))
           .html("<html><body><h1>IT'S IMPORTANT</h1></body></html>")))
           .onSuccess {
             case _ => println("delivered report")
           }*/
        .content(Multipart().html(template(content)))).onSuccess {
          case _ => {}
        }
  }

  def template(content: String): String = s"""<html>

  <body class="" style="font-family: sans-serif; -webkit-font-smoothing: antialiased; font-size: 14px; line-height: 1.4; -ms-text-size-adjust: 100%; -webkit-text-size-adjust: 100%; background-color: white; margin: 0; padding: 0;">
    <table border="0" cellpadding="0" cellspacing="0" class="body" style="border-collapse: separate; mso-table-lspace: 0pt; mso-table-rspace: 0pt; width: 100%; " width="100%" >
      <tr>
        <td style="font-family: sans-serif; font-size: 14px; vertical-align: top;" valign="top">&nbsp;</td>
        <td class="container" style="font-family: sans-serif; font-size: 14px; vertical-align: top; display: block; Margin: 0 auto !important; max-width: 580px; padding: 10px; width: 580px;" width="580" valign="top">
          <div class="content" style="box-sizing: border-box; display: block; Margin: 0 auto; max-width: 580px; padding: 10px;">

            <!-- START CENTERED WHITE CONTAINER -->
            <span class="preheader" style="color: transparent; display: none; height: 0; max-height: 0; max-width: 0; opacity: 0; overflow: hidden; mso-hide: all; visibility: hidden; width: 0;">This is preheader text. Some clients will show this text as a preview.</span>
            <table class="main" style="border-collapse: separate; mso-table-lspace: 0pt; mso-table-rspace: 0pt; width: 100%; background: #fff; border-radius: 3px;" width="100%">

              <!-- START MAIN CONTENT AREA -->
              <tr>
                <td class="wrapper" style="font-family: sans-serif; vertical-align: top; box-sizing: border-box; padding: 2vw;" valign="top">
                """ + content + s"""
                </td>
              </tr>
              <!-- END MAIN CONTENT AREA -->
              </table>

            <!-- START FOOTER -->
            <div class="footer" style="clear: both; padding-top: 10px; text-align: center; width: 100%;">
              <table border="0" cellpadding="0" cellspacing="0" style="border-collapse: separate; mso-table-lspace: 0pt; mso-table-rspace: 0pt; width: 100%;" width="100%">
                <tr>
                  <td class="content-block powered-by" style="font-family: sans-serif; vertical-align: top; padding-top: 10px; padding-bottom: 10px; font-size: 12px; color: #999999; text-align: center;" valign="top" align="center">
                    <p>
                    Copyright &#9400 2017. All rights reserved.
                    </p>
                  </td>
                </tr>
              </table>
            </div>

            <!-- END FOOTER -->

<!-- END CENTERED WHITE CONTAINER --></div>
        </td>
        <td style="font-family: sans-serif; font-size: 14px; vertical-align: top;" valign="top">&nbsp;</td>
      </tr>
    </table>
  </body>"""
}
