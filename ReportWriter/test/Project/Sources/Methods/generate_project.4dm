//%attributes = {}
$params:=New object:C1471
$params.PRODUCT_NAME:="ReportWriter"
$params.PRODUCT_VERSION:="2.0.0"
$params.AUTHOR:="INFORCE sro"
$params.CREATE_DATE:=Current date:C33
$params.COPYRIGHT_YEAR:=2024

generate_project_source($params)
generate_project_vs($params)
generate_project_xcode($params)
generate_project_plugin_stub($params)