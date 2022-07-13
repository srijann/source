

TResult TWtellDlgClient::Dispatch(TEventInfo& info, TParam1 wp, TParam2 lp)
{
  TResult result;

  result = TDialog::Dispatch(info, wp, lp);

  // INSERT>> Your code here.

  return result;
}
