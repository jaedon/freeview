# Figure out what V3D core revision we have based on PLATFORM and BCHP_VER
#
ifeq ($(PLATFORM),97231)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B0
  else ifeq ($(BCHP_VER),B0)
     V3D_CORE_REV := B1
  else ifeq ($(BCHP_VER),B1)
     V3D_CORE_REV := B1
  else ifeq ($(BCHP_VER),B2)
     V3D_CORE_REV := B1
  endif

else ifeq ($(PLATFORM),97429)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B1
  else ifeq ($(BCHP_VER),B0)
     V3D_CORE_REV := B2
  endif

else ifeq ($(PLATFORM),97428)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B1
  endif

else ifeq ($(PLATFORM),97241)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B1
  else ifeq ($(BCHP_VER),B0)
     V3D_CORE_REV := B2
  endif

else ifeq ($(PLATFORM),97344)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B0
  else ifeq ($(BCHP_VER),B0)
     V3D_CORE_REV := B1
  else ifeq ($(BCHP_VER),B1)
     V3D_CORE_REV := B1
  endif

else ifeq ($(PLATFORM),97346)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B0
  else ifeq ($(BCHP_VER),B0)
     V3D_CORE_REV := B1
  endif

else ifeq ($(PLATFORM),97421)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B0
  endif

else ifeq ($(PLATFORM),97422)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B0
  endif

else ifeq ($(PLATFORM),97424)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B0
  endif

else ifeq ($(PLATFORM),97425)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B0
  else ifeq ($(BCHP_VER),A1)
     V3D_CORE_REV := B0
  else ifeq ($(BCHP_VER),B0)
     V3D_CORE_REV := B1
  else ifeq ($(BCHP_VER),B1)
     V3D_CORE_REV := B1
  else ifeq ($(BCHP_VER),B2)
     V3D_CORE_REV := B1
  endif

else ifeq ($(PLATFORM),97435)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B2
  endif

else ifeq ($(PLATFORM),97640)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B0
  else ifeq ($(BCHP_VER),B0)
     V3D_CORE_REV := B1
  endif

else ifeq ($(PLATFORM),935230)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := A1
  else ifeq ($(BCHP_VER),C0)
     V3D_CORE_REV := B0
  endif

else ifeq ($(PLATFORM),935233)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B1
  endif

else ifeq ($(PLATFORM),935330)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B1
  endif

else ifeq ($(PLATFORM),97584)

  ifeq ($(BCHP_VER),A0)
     V3D_CORE_REV := B2
  endif

endif
