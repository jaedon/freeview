<?xml version='1.0'?>
<xsl:stylesheet	version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="CUTEST_LIST_REPORT">
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
				<b> Unitary Test </b> <br/>
			</h3>
		</div>
	</xsl:template>

	<xsl:template match="CUTEST_LIST_SUMMARY">
		<p/>
		<table align="center" width="50%">
			<xsl:apply-templates/>
		</table>
	</xsl:template>

	<xsl:template match="CUTEST_LIST_SUMMARY_RECORD">
		<tr>
			<td bgcolor="#f0f0e0" width="70%">
				<xsl:value-of select="CUTEST_LIST_SUMMARY_RECORD_TEXT" />
			</td>
			<td bgcolor="#f0e0e0" align="center">
				<xsl:value-of select="CUTEST_LIST_SUMMARY_RECORD_VALUE" />
			</td>
		</tr>
	</xsl:template>

	<xsl:template match="CUTEST_LIST_ALL_TEST_LIST">
		<p/>
		<div align="center">
			<h2> Listing of Registered Suites &amp; Tests </h2>
		</div>
		<table align="center" width="90%">
			<tr bgcolor="#00ccff">
				<td colspan="8"> </td>
			</tr>
			<tr>
				<td width="37%" colspan="2" bgcolor="7A9CED" align="center"> <b> Test Function </b> </td>
				<td width="7%" bgcolor="7A9CED" align="center"> <b> Active? </b> </td>
				<td width="56%" bgcolor="7A9CED" align="center"> <b> Description </b> </td>				
			</tr>
			<xsl:apply-templates/>
		</table>
	</xsl:template>

	<xsl:template match="CUTEST_LIST_TEST_CASE_DEFINITION">
		<tr>
			<td width="30%" colspan="2" bgcolor="#EBCB9C">
				<xsl:value-of select="CUTEST_LIST_TEST_CASE_NAME" />
			</td>
			<td width="10%" bgcolor="#e0e0d0" align="center">
				<xsl:value-of select="CUTEST_LIST_TEST_ACTIVE_VALUE" />
			</td>
			<td width="60%" bgcolor="#FFFECA">
				<pre>
				<xsl:call-template name="line">
				<xsl:with-param name="str" select="CUTEST_LIST_TEST_DESCRIPTION" />
				</xsl:call-template>
				</pre>				
			</td>			
		</tr>
	</xsl:template>

	<xsl:template match="CUTEST_FOOTER">
		<p/>
		<hr align="center" width="100%" color="maroon" />
		<h5 align="center">
			<xsl:apply-templates/>
		</h5>
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
