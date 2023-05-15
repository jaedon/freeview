<?xml version='1.0'?>
<xsl:stylesheet
	version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="CUTEST_RUN_REPORT">
		<html>
			<head>
				<title> Unitary Test </title>
			</head>

			<body bgcolor="#e0e0f0">
				<xsl:apply-templates/>
			</body>
		</html>
	</xsl:template>

	<xsl:template match="CUTEST_HEADER">
		<div align="center">
			<h3>
				<b> Unitary test </b> <br/>
			</h3>
		</div>
	</xsl:template>

	<xsl:template match="CUTEST_RESULT_ALL_RESULT_LIST">
		<p/>
		<div align="center">
			<h2> Automated Test Run Results </h2>
		</div>
		<table cols="4" width="90%" align="center">
			<tr>
				<td width="25%"> </td>
				<td width="25%"> </td>
				<td width="25%"> </td>
				<td width="25%"> </td>
			</tr>
			<xsl:apply-templates/>
		</table>
	</xsl:template>



	<xsl:template match="SUITE_NAME">
	</xsl:template>
	
	<xsl:template match="CUTEST_RESULT_RUN_TEST_RECORD">
		<xsl:apply-templates/>
	</xsl:template>

	<xsl:template match="CUTEST_RESULT_RUN_TEST_SUCCESS">
		<tr bgcolor="#e0f0d0">
			<td> </td>
			<td colspan="2">
				Running test <xsl:apply-templates/>...
			</td>
			<td bgcolor="#50ff50"> Passed </td>
		</tr>
	</xsl:template>

	<xsl:template match="CUTEST_RESULT_RUN_TEST_FAIL">
		<tr bgcolor="#e0f0d0">
			<td> </td>
			<td colspan="2">
				Running test <xsl:value-of select="CUTEST_RESULT_RUN_TEST_NAME"/>...
			</td>
			<td bgcolor="#ff5050"> Failed </td>
		</tr>

		<tr>
			<td colspan="4" bgcolor="#ff9090">
				<table width="100%">
					<tr>
						<th width="15%"> Error Locaion </th>
						<td width="85%" bgcolor="#e0eee0">
							<xsl:call-template name="line">
							<xsl:with-param name="str" select="CUTEST_RESULT_RUN_ERROR_LOCATION" />
							</xsl:call-template>
						</td>
					</tr>
					<tr>
						<th width="15%"> Condition </th>
						<td colspan="3" width="85%" bgcolor="#e0eee0">
						<pre>
							<xsl:call-template name="line">
							<xsl:with-param name="str" select="CUTEST_RESULT_RUN_CONDITION" />
							</xsl:call-template>
						</pre>
						</td>
					</tr>
				</table>
			</td>
		</tr>
	</xsl:template>

	

	<xsl:template match="CUTEST_RESULT_RUN_SUMMARY">
		<p/>
		<table width="90%" rows="5" align="center">
			<tr align="center" bgcolor="skyblue">
				<th colspan="6"> Cumulative Summary for Run </th>
			</tr>
			<tr>
				<th width="15%" bgcolor="#ffffc0" align="center"> Type </th>
				<th width="17%" bgcolor="#ffffc0" align="center"> Total </th>
				<th width="17%" bgcolor="#ffffc0" align="center"> Run </th>
				<th width="17%" bgcolor="#ffffc0" align="center"> Succeeded </th>
				<th width="17%" bgcolor="#ffffc0" align="center"> Failed </th>
			</tr>
			<xsl:for-each select="CUTEST_RESULT_RUN_SUMMARY_RECORD">
				<tr align="center" bgcolor="lightgreen">
					<td> <xsl:value-of select="CUTEST_RESULT_RUN_SUMMARY_TYPE" /> </td>
					<td> <xsl:value-of select="TOTAL" /> </td>
					<td> <xsl:value-of select="CUTEST_RESULT_RUN_SUMMARY_RUN" /> </td>
					<td> <xsl:value-of select="CUTEST_RESULT_RUN_SUMMARY_SUCCEEDED" /> </td>
					<td> <xsl:value-of select="CUTEST_RESULT_RUN_SUMMARY_FAILED" /> </td>
				</tr>
			</xsl:for-each>
		</table>
	</xsl:template>

	<xsl:template match="CUTEST_FOOTER">
		<p/>
		<hr align="center" width="90%" color="maroon" />
		<h5 align="center"> <xsl:apply-templates/> </h5>
	</xsl:template>

	
	<xsl:template name="line">
		<xsl:param name="str" />
		<xsl:choose>
			<xsl:when test="string-length($str)">
				<xsl:value-of select="substring-before($str, '&#13;')" /><br />
				<xsl:call-template name="line">
					<xsl:with-param name="str"><xsl:value-of select="substring-after($str, '&#13;')" /></xsl:with-param>
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
</xsl:stylesheet>
