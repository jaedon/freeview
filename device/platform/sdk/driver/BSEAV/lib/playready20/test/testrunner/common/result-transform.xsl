<?xml version="1.0" encoding="utf-8"?>

<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="html" />

<xsl:template match="/LOGSUMMARY">
  <xsl:variable name="log_file" select="@stdout_log" />
  <xsl:variable name="area_name" select="substring-before($log_file,'.htm')" />
    <html>
    <head>
	<link rel="stylesheet" type="text/css" href="result-style.css" />
      <title>
        <xsl:value-of select="$area_name" /> Results
      </title>
    </head>
    <body>
      <h1>
        <xsl:value-of select="$area_name" />
      </h1>
      <table cellspacing="0">
        <tr>
          <th class="total_cases">Total</th>
          <th class="total_passed">Pass</th>
          <th class="total_failed">Fail</th>
          </tr>
        <tr>  
          <td class="total_cases">
            <xsl:value-of select="count(LOGRESULTS)" />
          </td>
          <td class="total_passed">
            <xsl:value-of select="count(LOGRESULTS[@Result='PASS'])" />
          </td>
          <td class="total_failed">
            <xsl:value-of select="count(LOGRESULTS[@Result='FAIL'])" />
          </td>
        </tr>
      </table>
      <h2>Results</h2>
      <table cellspacing="0">
        <tr class="header">
          <th>ID</th>
          <th>Title</th>
          <th>Time (ms)</th>
        </tr>
        <xsl:for-each select="LOGRESULTS[@Result='FAIL']">
          <xsl:sort data-type="number" order="ascending" select="@CaseNo"/>
          <tr class="fail">
            <td>
              <a href="{$log_file}#{@CaseNo}" > <xsl:value-of select="@CaseNo" /></a>
            </td>
            <td>
              <xsl:value-of select="Title" />
              <div class="trace">
                <xsl:value-of select="Trace"/>
              </div>
            </td>
            <td>
              <xsl:value-of select="@Ticks" />
            </td>
          </tr>
        </xsl:for-each>
        <xsl:for-each select="LOGRESULTS[@Result='PASS']">
          <xsl:sort data-type="number" order="ascending" select="@CaseNo"/>

          <tr class="pass">
            <td>
              <a href="{$log_file}#{@CaseNo}" > <xsl:value-of select="@CaseNo" /></a>
            </td>
            <td>
              <xsl:value-of select="Title" />
            </td>
            <td>
              <xsl:value-of select="@Ticks" />
            </td>
          </tr>

        </xsl:for-each>
      </table>
    </body>
    </html>
</xsl:template>
  
</xsl:stylesheet> 
