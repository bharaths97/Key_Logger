Param( [Sting]$Attach_Main, [String]$Subject_Main, [String]$Body_Main )

Function Send-Mail
{
    Param(
            [Parameter(`
            Mandatory = $true)] 
        [String]$To, 
            [Parameter(`
            Mandatory = $true)]
        [String]$From, 
            [Parameter(`
            Mandatory = $true)]
        [String]$Password,
            [Parameter(`
            Mandatoy = $true)]
        [String]$Subject,
            [Prameter(`
            Mandatory = $true)]
        [String]$Body,
            [Parameter(`
            Mandatory = $true)]
        [String]$Attachments
        )

    try
    {
        $Message = New-Object System.Net.Mail.MailMessage($From,$To,$Subject,$Body)
        $Server = "smtp.gmail.com"

        if($Attachments -ne $null)
        {
            try
            {
                $Attachments_1 = $Attachments -split ("\:\:");

                ForEach($Val in  $Attachments_1)
                {
                    $attach = New-Object System.Net.Mail.Attachment($Val)
                    $Message.Attachments.Add($attach)
                }
            }
            catch
            {
                exit 2;
            }

            $client  = New-Object Net.Mail.SmtpClient($Server,587) #Specific for Gmail. Can be changed according to requirement
            $client.EnableSsl = $true
            $client.Credentials = New-Object System.Net.NetworkCredential($From.Split("@")[0],$Password);
            $client.Send($Message)
            Remove-Variable -name client
            Remove-Variable -name Password
            exit 7;   
        }
    }
    catch
    {
        exit 3;
    }
}

try
{
    Send-Mail
        -Attachments $Attach_Main
        -To "destination mail"
        -Body $Body_Main
        -Subject $Subject_Main
        -From "Source mail"
        -Password "Source mail password"
}
catch
{
    exit 4;
}