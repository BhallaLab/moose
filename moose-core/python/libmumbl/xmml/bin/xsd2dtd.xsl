<?xml version="1.0"?>
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0" xmlns:xs="http://www.w3.org/2001/XMLSchema">

  <xsl:output method="text"/>

  <xsl:strip-space elements="*"/>

  <!-- A few common block-level elements encountered in
       documentation. -->
  <xsl:template match="para | title | refpurpose">
    <xsl:apply-templates/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="indexterm"/>

  <xsl:template match="xs:attribute" mode="attributes">
    <xsl:text>&#xa;  </xsl:text>
    <xsl:choose>
      <xsl:when test="@name">
	<xsl:value-of select="@name"/>
	<xsl:choose>
	  <xsl:when test="xs:simpleType">
	    <xsl:apply-templates select="xs:simpleType"/>
	  </xsl:when>
	  <xsl:when test="@type">
	    <xsl:call-template name="simpleType">
	      <xsl:with-param name="type" select="@type"/>
	    </xsl:call-template>
	  </xsl:when>
	</xsl:choose>
      </xsl:when>
      <xsl:when test="@ref">
	<xsl:value-of select="substring-after(@ref,':')"/>
	<xsl:if test="@ref = 'xml:lang'">
	  <xsl:text> CDATA</xsl:text>
	</xsl:if>
      </xsl:when>
    </xsl:choose>
    <xsl:choose>
      <xsl:when test="@use = 'required'">
        <xsl:text> #REQUIRED</xsl:text>
      </xsl:when>
      <xsl:when test="@default">
        <xsl:text> "</xsl:text>
        <xsl:value-of select="@default"/>
        <xsl:text>"</xsl:text>
      </xsl:when>
      <xsl:when test="@fixed">
        <xsl:text> #FIXED "</xsl:text>
        <xsl:value-of select="@fixed"/>
        <xsl:text>"</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text> #IMPLIED</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xs:attribute[@ref][xs:annotation/xs:documentation]" mode="attr-comments">
    <xsl:text>&#xa;</xsl:text>
    <xsl:value-of select="substring-after(@ref,':')"/>
    <xsl:text>: </xsl:text>
    <xsl:value-of select="xs:annotation/xs:documentation"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="xs:attribute[xs:annotation/xs:documentation]" mode="attr-comments">
    <xsl:text>&#xa;</xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text>: </xsl:text>
    <xsl:value-of select="xs:annotation/xs:documentation"/>
    <xsl:text>&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="xs:attribute"/>

  <xsl:template match="xs:attributeGroup[@name]"/>

  <xsl:template match="xs:attributeGroup[@ref]" mode="attributes">
    <xsl:text>&#xa;  %</xsl:text>
    <xsl:value-of select="substring-after(@ref,':')"/>
    <xsl:text>;</xsl:text>
  </xsl:template>

  <xsl:template match="xs:attributeGroup[@name]" mode="groups">
    <xsl:text>&#xa;&#xa;&lt;!-- ATTRIBUTE GROUP </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text> --></xsl:text>
    <xsl:apply-templates select="xs:annotation"/>
    <xsl:text>&#xa;&lt;!--</xsl:text>
    <xsl:apply-templates select="descendant::xs:attribute"
      mode="attr-comments"/>
    <xsl:text>-->&#xa;</xsl:text>
    <xsl:text>&#xa;&lt;!ENTITY % </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text> "</xsl:text>
    <xsl:apply-templates select="descendant::xs:attribute"
      mode="attributes"/>
    <xsl:text> " ></xsl:text>
  </xsl:template>

  <xsl:template match="xs:choice">
    <xsl:if test="not(parent::xs:group)">
      <xsl:text> (</xsl:text>
    </xsl:if>
    <xsl:for-each select="*">
      <xsl:apply-templates select="."/>
      <xsl:if test="position() != last()">
        <xsl:text> |</xsl:text>
      </xsl:if>
    </xsl:for-each>
    <xsl:if test="not(parent::xs:group)">
      <xsl:text> )</xsl:text>
    </xsl:if>
    <xsl:call-template name="occurrence"/>
  </xsl:template>

  <xsl:template match="xs:complexType">
    <xsl:choose>
      <xsl:when test="xs:all">
        <xsl:text> ANY</xsl:text>
      </xsl:when>
      <xsl:when test="xs:choice | xs:complexContent | xs:group |
                      xs:sequence | xs:simpleContent">
        <xsl:choose>
          <xsl:when test="@mixed='true'">
            <xsl:text> ( #PCDATA </xsl:text>
            <xsl:for-each select="descendant::xs:group |
                                  descendant::xs:element">
              <xsl:text>| </xsl:text>
              <xsl:if test="self::xs:group">
                <xsl:text>%</xsl:text>
              </xsl:if>
              <xsl:value-of select="substring-after(@ref,':')"/>
              <xsl:if test="self::xs:group">
                <xsl:text>;</xsl:text>
              </xsl:if>
            </xsl:for-each>
            <xsl:text> )*</xsl:text>
          </xsl:when>
          <xsl:otherwise>
            <xsl:apply-templates/>            
          </xsl:otherwise>
        </xsl:choose>
      </xsl:when>
      <xsl:when test="@mixed='true'">
        <xsl:text> ( #PCDATA )</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text> EMPTY</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xs:documentation">
    <xsl:text>&#xa;&lt;!--&#xa;</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#xa;--></xsl:text>
  </xsl:template>

  <xsl:template match="xs:element[@name]">
    <xsl:if test="not(preceding-sibling::xs:element)">
      <xsl:apply-templates select="../xs:group" mode="groups"/>
      <xsl:apply-templates select="../xs:attributeGroup"
        mode="groups"/>
    </xsl:if>
    <xsl:text>&#xa;&#xa;&lt;!-- ELEMENT </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text> --></xsl:text>
    <xsl:apply-templates select="xs:annotation"/>
    <xsl:text>&#xa;&lt;!ELEMENT </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:choose>
      <xsl:when test="xs:complexType">
        <xsl:apply-templates select="xs:complexType"/>
      </xsl:when>
      <xsl:when test="xs:simpleType">
        <xsl:text> ( #PCDATA )</xsl:text>
      </xsl:when>
      <xsl:when test="@type">
        <xsl:text> ( #PCDATA )</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text> EMPTY</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text> ></xsl:text>
    <xsl:if
      test="descendant::xs:attribute | descendant::xs:attributeGroup">
      <xsl:text>&#xa;&lt;!--</xsl:text>
      <xsl:apply-templates
	select="descendant::xs:attribute |
		descendant::xs:attributeGroup"
        mode="attr-comments"/>
      <xsl:text>-->&#xa;</xsl:text>
    </xsl:if>
    <xsl:if test="/xs:schema/@targetNamespace |
                  descendant::xs:attribute |
		  descendant::xs:attributeGroup">
      <xsl:text>&lt;!ATTLIST </xsl:text>
      <xsl:value-of select="@name"/>
      <!--<xsl:if test="/xs:schema/@targetNamespace">
        <xsl:text>&#xa;  xmlns CDATA #FIXED "</xsl:text>
        <xsl:value-of select="/xs:schema/@targetNamespace"/>
        <xsl:text>"</xsl:text>
      </xsl:if>-->
      <xsl:if
	test="descendant::xs:attribute |
	      descendant::xs:attributeGroup">
        <xsl:apply-templates
	  select="descendant::xs:attribute |
		  descendant::xs:attributeGroup"
          mode="attributes"/>
      </xsl:if>
      <xsl:text> ></xsl:text>
    </xsl:if>
  </xsl:template>

  <xsl:template match="xs:element[@ref]">
    <xsl:text> </xsl:text>
    <xsl:value-of select="substring-after(@ref,':')"/>
    <xsl:call-template name="occurrence"/>
  </xsl:template>

  <xsl:template match="xs:group[@name]"/>

  <xsl:template match="xs:group[@name]" mode="groups">
    <xsl:text>&#xa;&#xa;&lt;!-- GROUP </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text> --></xsl:text>
    <xsl:apply-templates select="xs:annotation"/>
    <xsl:text>&#xa;&lt;!ENTITY % </xsl:text>
    <xsl:value-of select="@name"/>
    <xsl:text> "</xsl:text>
    <xsl:apply-templates select="xs:choice | xs:complexContent |
                                 xs:group | xs:sequence |
                                 xs:simpleContent"/>
    <xsl:text> " ></xsl:text>
  </xsl:template>

  <xsl:template match="xs:group[@ref]">
    <xsl:text> (%</xsl:text>
    <xsl:value-of select="substring-after(@ref,':')"/>
    <xsl:text>;)</xsl:text>
    <xsl:call-template name="occurrence"/>
  </xsl:template>

  <xsl:template match="xs:import">
    <xsl:text>&#xa;&#xa;&lt;!-- IMPORT:&#xa;</xsl:text>
    <xsl:text>     namespace: </xsl:text>
    <xsl:value-of select="@namespace"/>
    <xsl:text>&#xa;     URI: </xsl:text>
    <xsl:value-of select="@schemaLocation"/>
    <xsl:text> --></xsl:text>
    <xsl:apply-templates select="xs:annotation"/>
  </xsl:template>

  <xsl:template match="xs:include">
    <xsl:variable name="sysent-name">
      <xsl:choose>
        <xsl:when test="document(@schemaLocation)/xs:schema/@id">
          <xsl:value-of
            select="document(@schemaLocation)/xs:schema/@id"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="translate(@schemaLocation, ' ', '')"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:apply-templates select="xs:annotation"/>
    <xsl:text>&#xa;&lt;!ENTITY % </xsl:text>
    <xsl:value-of select="$sysent-name"/>
    <xsl:text> SYSTEM "</xsl:text>
    <xsl:value-of
      select="substring-before(@schemaLocation, '.xsd')"/>
    <xsl:text>.dtd" >&#xa;</xsl:text>
    <xsl:text>%</xsl:text>
    <xsl:value-of select="$sysent-name"/>
    <xsl:text>;&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="xs:restriction">
    <xsl:choose>
      <xsl:when test="xs:enumeration">
        <xsl:text> ( </xsl:text>
        <xsl:for-each select="xs:enumeration">
          <xsl:value-of select="@value"/>
          <xsl:if test="position() != last()">
            <xsl:text> | </xsl:text>
          </xsl:if>
        </xsl:for-each>
        <xsl:text> )</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:call-template name="simpleType">
          <xsl:with-param name="type" select="@base"/>
        </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template match="xs:schema">
    <xsl:text>&lt;!-- Note that the original schema </xsl:text>
    <xsl:text>declarations may be stricter than&#xa;</xsl:text>
    <xsl:text>     represented here; please see the </xsl:text>
    <xsl:text>documentation for each element&#xa;</xsl:text>
    <xsl:text>     type. -->&#xa;</xsl:text>
    <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="xs:sequence">
    <xsl:if test="not(parent::xs:group)">
      <xsl:text> (</xsl:text>
    </xsl:if>
    <xsl:for-each select="*">
      <xsl:apply-templates select="."/>
      <xsl:if test="position() != last()">
        <xsl:text>,</xsl:text>
      </xsl:if>
    </xsl:for-each>
    <xsl:if test="not(parent::xs:group)">
      <xsl:text> )</xsl:text>
    </xsl:if>
    <xsl:call-template name="occurrence"/>
  </xsl:template>

  <xsl:template match="xs:schema/xs:simpleType"/>

  <xsl:template match="xs:simpleType">
    <xsl:choose>
      <xsl:when test="xs:restriction/@base='xs:NMTOKEN' and
                      xs:restriction/xs:enumeration">
        <xsl:text> ( </xsl:text>
        <xsl:for-each select="xs:restriction/xs:enumeration">
          <xsl:value-of select="@value"/>
          <xsl:if test="position() != last()">
            <xsl:text> | </xsl:text>
          </xsl:if>
        </xsl:for-each>
        <xsl:text> )</xsl:text>
      </xsl:when>
      <xsl:when
        test="xs:list/@itemType='xs:NMTOKEN' or
              xs:list/xs:simpleType/xs:restriction/@base='xs:NMTOKEN'">
        <xs:text> NMTOKENS</xs:text>
      </xsl:when>
      <xsl:when
        test="xs:list/@itemType='xs:IDREF' or
              xs:list/xs:simpleType/xs:restriction/@base='xs:IDREF'">
        <xs:text> IDREFS</xs:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text> CDATA</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <xsl:template name="occurrence">
    <xsl:variable name="minOccurs">
      <xsl:choose>
        <xsl:when test="@minOccurs">
          <xsl:value-of select="@minOccurs"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="1"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="maxOccurs">
      <xsl:choose>
        <xsl:when test="@maxOccurs">
          <xsl:value-of select="@maxOccurs"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:value-of select="1"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="$minOccurs = 0 and $maxOccurs = 1">
        <xsl:text>?</xsl:text>
      </xsl:when>
      <xsl:when test="$minOccurs = 0">
        <xsl:text>*</xsl:text>
      </xsl:when>
      <xsl:when test="$maxOccurs = 1"/>
      <xsl:otherwise>
        <xsl:text>+</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- only used by attributes -->
  <xsl:template name="simpleType">
    <xsl:param name="type"/>
    <xsl:choose>
      <xsl:when test="$type = 'xs:boolean'">
        <xsl:text> ( true | false )</xsl:text>
      </xsl:when>
      <xsl:when test="$type = 'xs:Name' or $type = 'xs:NCName' or
                      $type = 'xs:NMTOKEN'">
        <xsl:text> NMTOKEN</xsl:text>
      </xsl:when>
      <xsl:when test="$type = 'xs:ID' or $type = 'xs:IDREF' or
                      $type = 'xs:IDREFS' or $type = 'xs:ENTITY' or
                      $type = 'xs:ENTITIES' or $type = 'xs:NMTOKENS'">
        <xsl:text> </xsl:text>
        <xsl:value-of select="substring-after($type,'xs:')"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text> CDATA</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

</xsl:transform>
